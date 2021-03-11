/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*             Copyright (C)1998-2021, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#include "sdk/menus/menu.h"

// ReSharper disable once CppUnusedIncludeDirective
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include "sdk/menus/menus_cereal.h"

#include "core/datafile.h"
#include "core/file.h"
#include "core/jsonfile.h"
#include "core/stl.h"
#include "core/strings.h"
#include "sdk/config.h"
#include "sdk/user.h"
#include "sdk/acs/acs.h"
#include "sdk/acs/expr.h"
#include <string>
#include <utility>
#include <vector>

using cereal::make_nvp;
using std::string;
using std::vector;
using namespace wwiv::core;
using namespace wwiv::stl;
using namespace wwiv::strings;

namespace wwiv::sdk::menus {

Menu430::Menu430(std::filesystem::path menu_dir, std::string menu_set, std::string menu_name)
    : menu_dir_(std::move(menu_dir)), menu_set_(std::move(menu_set)),
      menu_name_(std::move(menu_name)) {
  initialized_ = Load();
}

bool Menu430::Load() {
  const auto menu_dir = FilePath(menu_dir_, menu_set_);
  const auto menu_name = StrCat(menu_name_, ".mnu");
  const auto path = FilePath(menu_dir, menu_name);
  File menu_file(path);
  if (!menu_file.Open(File::modeBinary | File::modeReadOnly, File::shareDenyNone)) {
    // Unable to open menu
    return false;
  }

  // Read the header (control) record into memory
  menu_file.Read(&header, sizeof(menu_header_430_t));

  const auto num_items = static_cast<int>(menu_file.length() / sizeof(menu_rec_430_t));

  for (auto nn = 1; nn < num_items; nn++) {
    menu_rec_430_t menu{};
    menu_file.Read(&menu, sizeof(menu_rec_430_t));
    recs.emplace_back(menu);
  }
  initialized_ = true;
  return true;
}

menu_action_56_t CreateActionFrom43Execute(const std::string& exec) {
  menu_action_56_t a{};
  if (!contains(exec, ' ')) {
    a.cmd = ToStringLowerCase(exec);
    return a;
  }
  const auto space = exec.find(' ');
  a.cmd = ToStringLowerCase(exec.substr(0, space));
  auto data = StringTrim(exec.substr(space + 1));
  if (data.size() > 2) {
    if (data.back() == ')') {
      data.pop_back();
    }
    if (data.front() == '(') {
      data = data.substr(1);
    }
  }
  a.data = data;
  return a;
}

Menu56::Menu56(std::filesystem::path menu_dir, std::string menu_set, std::string menu_name)
    : menu_dir_(std::move(menu_dir)), menu_set_(std::move(menu_set)),
      menu_name_(std::move(menu_name)) {
  initialized_ = Load();
}

bool Menu56::Load() {
  const auto dir = FilePath(menu_dir_, menu_set_);
  const auto name = StrCat(menu_name_, ".mnu.json");
  JsonFile f(FilePath(dir, name), "menu", menu, 1);
  return f.Load();
}

bool Menu56::Save() {
  const auto dir = FilePath(menu_dir_, menu_set_);
  const auto name = StrCat(menu_name_, ".mnu.json");
  JsonFile f(FilePath(dir, name), "menu", menu, 1);
  return f.Save();
}

std::optional<Menu56> Create56MenuFrom43(const Menu430& m4, int max_backups) {
  const auto dir = FilePath(m4.menu_dir_, m4.menu_set_);
  const auto name = StrCat(m4.menu_name_, ".mnu.json");
  const auto path = FilePath(dir, name);
  if (backup_file(path, max_backups)) {
    File::Remove(path);
  }
  Menu56 m5(m4.menu_dir_, m4.menu_set_, m4.menu_name_);

  auto& h = m5.menu;
  const auto& oh = m4.header;
  h.logging_action = to_menu_logtype(oh.nLogging);
  h.num_action = to_menu_numflag_t(oh.nums);
  h.help_type = to_menu_help_display(oh.nForceHelp);

  auto& g = h.generated_menu;
  g.color_title = fmt::format("|{:02}", oh.nTitleColor);
  g.color_item_key = fmt::format("|{:02}", oh.nItemTextHLColor);
  g.color_item_braces = fmt::format("|{:02}", oh.nItemBorderColor);
  g.color_item_text = fmt::format("|{:02}", oh.nItemTextColor);

  h.title = oh.szMenuTitle;
  {
    acs::AcsExpr ae;
    ae.min_dsl(oh.nMinDSL).dar_int(oh.uDAR).min_sl(oh.nMinSL).ar_int(oh.uAR);
    ae.sysop(oh.nSysop).cosysop(oh.nCoSysop);
    h.acs = ae.get();
  }
  h.password = oh.szPassWord;
  if (oh.szScript[0]) {
    h.enter_actions.emplace_back(CreateActionFrom43Execute(oh.szScript));
  }
  if (oh.szExitScript[0]) {
    h.exit_actions.emplace_back(CreateActionFrom43Execute(oh.szScript));
  }

  // Add the items now.
  for (const auto& o : m4.recs) {
    menu_item_56_t i{};
    i.item_key = o.szKey;
    i.item_text = o.szMenuText;
    i.help_text = o.szHelp;
    i.log_text = o.szSysopLog;
    i.instance_message = o.szInstanceMessage;

    {
      // TODO(rushfan): restrict into ACS.
      acs::AcsExpr ae;
      ae.min_sl(o.nMinSL)
          .max_sl(o.iMaxSL)
          .min_dsl(o.nMinDSL)
          .max_dsl(o.iMaxDSL)
          .ar_int(o.uAR)
          .dar_int(o.uDAR);
      ae.sysop(o.nSysop).cosysop(o.nCoSysop);
      i.acs = ae.get();
    }

    i.password = o.szPassWord;
    i.actions.emplace_back(CreateActionFrom43Execute(o.szExecute));

    h.items.emplace_back(i);
  }

  m5.set_initialized(true);
  return {m5};
}

// What does a number key do in this menu.
menu_numflag_t to_menu_numflag_t(int n) {
  switch (n) {
  case MENU_NUMFLAG_NOTHING:
    return menu_numflag_t::none;
  case MENU_NUMFLAG_DIRNUMBER:
    return menu_numflag_t::dirs;
  case MENU_NUMFLAG_SUBNUMBER:
    return menu_numflag_t::subs;
  default:
    return menu_numflag_t::none;
  }
}

// What to log for this command.
menu_logtype_t to_menu_logtype(int n) {
  switch (n) {
  case MENU_LOGTYPE_NONE:
    return menu_logtype_t::none;
  case MENU_LOGTYPE_COMMAND:
    return menu_logtype_t::command;
  case MENU_LOGTYPE_DESC:
    return menu_logtype_t::description;
  case MENU_LOGTYPE_KEY:
    return menu_logtype_t::key;
  default:
    return menu_logtype_t::none;
  }
}

menu_help_display_t to_menu_help_display(int n) {
  if (n == MENU_HELP_DONTFORCE) {
    return menu_help_display_t::user_choice;
  }
  if (n == MENU_HELP_ONENTRANCE) {
    return menu_help_display_t::on_entrance;
  }
  return menu_help_display_t::always;
}

std::vector<menu_command_help_t> LoadCommandHelpJSON(const std::string& datadir) {
  std::vector<menu_command_help_t> cmds;
  const auto path = FilePath(datadir, "menu_commands.json");
  JsonFile f(path, "commands", cmds, 1);
  if (!f.Load()) {
    return {};
  }
  return cmds;

}

bool SaveCommandHelpJSON(const std::string& datadir, const std::vector<menu_command_help_t>& cmds) {
  VLOG(1) << "SaveCommandHelpJSON";
  const auto path = FilePath(datadir, "menu_commands.json");
  JsonFile f(path, "commands", cmds, 1);
  return f.Save();
}

/**
 * Returns a string to display for a key.
 * Examples:
 *   'A' for a key 'A'
 *   '/A' for a key '/A'
 *   '//KEY' for a key 'KEY'
 */
static std::string display_key(const std::string& item_key) {
  if (item_key.size() == 1) {
    return item_key;
  }
  if (item_key.size() == 2 && item_key.front() == '/') {
    return item_key;
  }
  if (item_key.size() == 2) {
    return fmt::format("//{}", item_key);
  }
  if (starts_with(item_key, "//")) {
    return item_key;
  }
  return fmt::format("//{}", item_key);
}

static std::string generate_menu_item_line(generated_menu_56_t g, const std::string& key,
                                           const std::string& text, int max_width) {
  std::ostringstream ss;
  ss << g.color_item_braces << "(" << g.color_item_key << display_key(key) << g.color_item_braces
     << ")" << g.color_item_text;
  if (key.size() == 1 && !text.empty() &&
      to_upper_case_char(text.front()) == to_upper_case_char(key.front())) {
    ss << text.substr(1);
  } else {
    ss << " " << text;
  }
  ss << " ";
  const auto line = trim_to_size_ignore_colors(ss.str(), max_width);
  const auto len = size_without_colors(line);
  return fmt::format("{}{}", line, std::string(max_width - len, ' '));
}

std::vector<std::string> GenerateMenuLines(const Config& config, int eff_sl, const menu_56_t& menu, const sdk::User& user, menu_type_t typ) {
  std::vector<std::string> out;
  out.emplace_back("|#0");

  auto lines_displayed = 0;
  const auto& g = menu.generated_menu;
  const auto& title = menu.title;
  const auto num_cols = typ == menu_type_t::short_menu ? g.num_cols : 1;
  const auto screen_width = user.GetScreenChars() - num_cols + 1;
  const auto col_width =
      typ == menu_type_t::short_menu ? screen_width / num_cols : screen_width - 1;
  if (!title.empty()) {
    const auto title_len = size_without_colors(title);
    const auto pad_len = (screen_width - title_len) / 2;
    out.emplace_back(fmt::format("{}{}{}|#0", std::string(pad_len, ' '), g.color_title, title));
  }
  std::ostringstream ss;
  if (menu.num_action != menu_numflag_t::none) {
    ss << generate_menu_item_line(g, "#", "Change Sub/Dir #", col_width);
    ++lines_displayed;
  }
  auto just_nled = false;
  for (const auto& mi : menu.items) {
    if (mi.item_key.empty()) {
      continue;
    }
    acs::UserValueProvider up(config, user, eff_sl, config.sl(eff_sl));
    if (auto [result, debug_lines] = check_acs(config, up, mi.acs); !result) {
      continue;
    }
    if (!g.show_empty_text && StringTrim(mi.item_text).empty()) {
      continue;
    }
    if (!mi.visible) {
      continue;
    }
    just_nled = false;
    const auto key = display_key(mi.item_key);
    const auto& text = typ == menu_type_t::short_menu ? mi.item_text : mi.help_text;
    ss << generate_menu_item_line(g, key, text, col_width);
    if (++lines_displayed % num_cols == 0) {
      out.emplace_back(ss.str());
      ss.str("");
      ss.clear();
      just_nled = true;
    }
  }
  if (!just_nled) {
    out.emplace_back(ss.str());
    ss.str("");
    ss.clear();
  }
  for (auto i = 0; i < g.num_newlines_at_end; i++) {
    out.emplace_back("");
  }
  return out;
}


} // namespace wwiv::sdk::menus
