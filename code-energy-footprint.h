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
 * Opens all needed counters for a piece of code (represented as a label).
 * These counters are represented as file descriptors and 
 * are sent to PowerAPI via Unix Domain sockets for speed reason.
 * The sending is assured by libancillary (jnaerated in PowerAPI for the receiving).
 */
extern void configure_all_pc(char *label);

/**
 * Starts to feed the counters associated to a label.
 */
extern void start_measure(char *label);

/**
 * Stop to feed the counters associated to a label.
 */
extern void stop_measure(char *label);

