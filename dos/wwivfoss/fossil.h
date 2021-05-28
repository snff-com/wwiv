/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5                            */
/*                  Copyright (C)2021, WWIV Software Services             */
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
#ifndef INCLUDED_FOSSIL_H
#define INCLUDED_FOSSIL_H

#include "dostypes.h"

#define ENABLE_LOG
#define FOSSIL_BUFFER_SIZE 4000

class FossilOptions {
public:
  FossilOptions();
  ~FossilOptions();

  int comport;
  int node_number;
  int open_timeout_seconds;
};

bool enable_fossil(const FossilOptions* options);
bool disable_fossil();

#endif // INCLUDED_FOSSIL_H
