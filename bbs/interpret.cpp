/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
/*             Copyright (C)1998-2004, WWIV Software Services             */
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

#include "wwiv.h"
#include "WStringUtils.h"


char *interpret( char chKey )
{
    static char s[255];

    memset( s, 0, sizeof( s ) );
    if (g_flags & g_flag_disable_mci)
    {
        return "";
    }

    switch ( chKey )
    {
    case '@':                               // Dir name
        strcpy( s, directories[udir[sess->GetCurrentFileArea()].subnum].name );
        break;
    case '~':                               // Total mails/feedbacks sent
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumEmailSent() +
            sess->thisuser.GetNumFeedbackSent() + sess->thisuser.GetNumNetEmailSent() );
        break;
    case '/':                               // Today's date
        strcpy( s, fulldate() );
        break;
    case '%':                               // Time left today
        snprintf( s, sizeof( s ), "%d", static_cast<int>( nsl() / 60) );
        break;
    case '#':                               // User's number
        snprintf( s, sizeof( s ), "%lu", sess->usernum );
        break;
    case '$':                               // File points
        snprintf( s, sizeof( s ), "%lu", sess->thisuser.GetFilePoints() );
        break;
    case '*':                               // User reg num
        snprintf( s, sizeof( s ), "%lu", sess->thisuser.GetWWIVRegNumber() );
        break;
    case '-':                               // Aggravation points
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetAssPoints() );
        break;
    case ':':                               // Sub number
        strcpy( s, usub[sess->GetCurrentMessageArea()].keys );
        break;
    case ';':                               // Directory number
        strcpy( s, udir[sess->GetCurrentFileArea()].keys );
        break;
    case '!':                               // Built-in pause
        pausescr();
        break;
    case '&':
        {
            strcpy( s, sess->thisuser.hasAnsi() ? "ANSI" : "ASCII" );
        }
        break;
    case 'A':                               // User's age
        snprintf( s, sizeof( s ), "%d", sess->thisuser.GetAge() );
        break;
    case 'a':                               // User's language
        strcpy( s, cur_lang_name );
        break;
    case 'B':                               // User's birthday
        snprintf( s, sizeof( s ), "%d/%d/%d", sess->thisuser.GetBirthdayMonth(),
                 sess->thisuser.GetBirthdayDay(), sess->thisuser.GetBirthdayYear() );
        break;
    case 'b':                               // Minutes in bank
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetTimeBankMinutes() );
        break;
    case 'C':                               // User's city
        strcpy( s, sess->thisuser.GetCity() );
        break;
    case 'c':                               // User's country
        strcpy( s, sess->thisuser.GetCountry() );
        break;
    case 'D':                               // Files downloaded
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetFilesDownloaded() );
        break;
    case 'd':                               // User's DSL
        snprintf( s, sizeof( s ), "%d", sess->thisuser.GetDsl() );
        break;
    case 'E':                               // E-mails sent
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumEmailSent() );
        break;
    case 'e':                               // Net E-mails sent
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumNetEmailSent() );
        break;
    case 'F':
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumFeedbackSent() );
        break;
    case 'f':                               // First time user called
        strcpy( s, sess->thisuser.GetFirstOn() );
        break;
    case 'G':                               // MessaGes read
        snprintf( s, sizeof( s ), "%lu", sess->thisuser.GetNumMessagesRead() );
        break;
    case 'g':                               // Gold
        snprintf( s, sizeof( s ), "%f", sess->thisuser.GetGold() );
        break;
    case 'I':                               // User's call sIgn
        strcpy( s, sess->thisuser.GetCallsign() );
        break;
    case 'i':                               // Illegal log-ons
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumIllegalLogons() );
        break;
    case 'J':                               // Message conference
        strcpy( s, reinterpret_cast<char*>( subconfs[uconfsub[sess->GetCurrentConferenceMessageArea()].confnum].name ) );
        break;
    case 'j':                               // Transfer conference
        strcpy( s, reinterpret_cast<char*>( dirconfs[uconfdir[sess->GetCurrentConferenceFileArea()].confnum].name ) );
        break;
    case 'K':                               // Kb uploaded
        snprintf( s, sizeof( s ), "%lu", sess->thisuser.GetUploadK() );
        break;
    case 'k':                               // Kb downloaded
        snprintf( s, sizeof( s ), "%lu", sess->thisuser.GetDownloadK() );
        break;
    case 'L':                               // Last call
        strcpy( s, sess->thisuser.GetLastOn() );
        break;
    case 'l':                               // Number of logons
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumLogons() );
        break;
    case 'M':                               // Mail waiting
        snprintf( s, sizeof( s ), "%d", sess->thisuser.GetNumMailWaiting() );
        break;
    case 'm':                               // Messages posted
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetNumMessagesPosted() );
        break;
    case 'N':                               // User's name
        strcpy( s, sess->thisuser.GetName() );
        break;
    case 'n':                               // Sysop's note
        strcpy( s, sess->thisuser.GetNote() );
        break;
    case 'O':                               // Times on today
        snprintf( s, sizeof( s ), "%d", sess->thisuser.GetTimesOnToday() );
        break;
    case 'o':                               // Time on today
        snprintf( s, sizeof( s ), "%ld", static_cast<long>( ( sess->thisuser.GetTimeOn() +
            timer() - timeon ) /
            SECONDS_PER_MINUTE_FLOAT ) );
        break;
    case 'P':                               // BBS phone
        strcpy( s, reinterpret_cast<char*>( syscfg.systemphone ) );
        break;
    case 'p':                               // User's phone
        strcpy( s, sess->thisuser.GetDataPhoneNumber() );
        break;
    case 'R':                               // User's real name
        strcpy( s, sess->thisuser.GetRealName() );
        break;
    case 'r':                               // Last baud rate
        snprintf( s, sizeof( s ), "%d", sess->thisuser.GetLastBaudRate() );
        break;
    case 'S':                               // User's SL
        snprintf( s, sizeof( s ), "%d", sess->thisuser.GetSl() );
        break;
    case 's':                               // User's street address
        strcpy( s, sess->thisuser.GetStreet() );
        break;
    case 'T':                               // User's sTate
        strcpy( s, sess->thisuser.GetState() );
        break;
    case 't':                               // Current time
        strcpy( s, times() );
        break;
    case 'U':                               // Files uploaded
        snprintf( s, sizeof( s ), "%u", sess->thisuser.GetFilesUploaded() );
        break;
    case 'u':                               // Current sub
        strcpy( s, subboards[usub[sess->GetCurrentMessageArea()].subnum].name );
        break;
    case 'W':                               // Total # of messages in sub
        snprintf( s, sizeof( s ), "%ld", sess->GetNumMessagesInCurrentMessageArea() );
        break;
    case 'X':                               // User's sex
        snprintf( s, sizeof( s ), "%c", sess->thisuser.GetGender() );
        break;
    case 'Y':                               // Your BBS name
        strcpy( s, syscfg.systemname );
        break;
    case 'y':                               // Computer type
        strcpy( s, ctypes( sess->thisuser.GetComputerType() ) );
        break;
    case 'Z':                               // User's zip code
        strcpy( s, sess->thisuser.GetZipcode() );
        break;
    default:
        return "";
    }

    return s;
}

