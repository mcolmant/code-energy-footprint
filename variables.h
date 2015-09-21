/**
 * Copyright 2015 - Maxime Colmant <maxime.colmant@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Unix domain socket server paths (servers have to be created by PowerAPI).
 * These sockets are used to send file descriptors from an application to PowerAPI.
 */
extern char *control_flow_server_path;
extern char *fd_flow_server_path;

/**
 * Hardware and formulae settings (they have to correspond to the parameters defined in PowerAPI).
 */
extern int NEVENTS;
extern int NCPUS;
extern char *events[];

