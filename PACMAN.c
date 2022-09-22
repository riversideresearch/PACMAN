/* Parser combinator for MORA Data Messages.
* Copyright (C) 2022  Thomas Bailey
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation, version 2.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <hammer/hammer.h>
#include <hammer/glue.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "test_suite.h"
#include "PACMAN.h"

int TYPE = 2; // for keeping track of which type we are testing

unsigned long ASCIILookupTable(int ch) {
    switch(ch) {
        case 48:
            return(0);
        case 49:
            return(1);
        case 50:
            return(2);
        case 51:
            return(3);
        case 52:
            return(4);
        case 53:
            return(5);
        case 54:
            return(6);
        case 55:
            return(7);
        case 56:
            return(8);
        case 57:
            return(9);
    }
}

unsigned long concatenate(unsigned long x, unsigned long y) {
    unsigned long pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;        
}

HParsedToken *act_nDigitInt(int n, const HParseResult *p, void *user_data) { // n is unused, but necessary because of how H_ACT_APPLY works
    const HParsedToken *seq, *elem;
    seq = p->ast;
    unsigned long value = h_seq_index(seq, 0)->uint;
    value = ASCIILookupTable(value);
    unsigned long holder;
    for (unsigned int i = 1; i < h_seq_len(seq); i++) {
        elem = h_seq_index(seq, i);
        holder = elem->uint;
        holder = ASCIILookupTable(holder);
        value = concatenate(value, holder);
    }

    return H_MAKE_UINT(value);
}

H_ACT_APPLY(act_oneDigitInt, act_nDigitInt, 1);
H_ACT_APPLY(act_twoDigitInt, act_nDigitInt, 2);
H_ACT_APPLY(act_threeDigitInt, act_nDigitInt, 3);
H_ACT_APPLY(act_fourDigitInt, act_nDigitInt, 4);
H_ACT_APPLY(act_fiveDigitInt, act_nDigitInt, 5);
H_ACT_APPLY(act_sevenDigitInt, act_nDigitInt, 7);
H_ACT_APPLY(act_eightDigitInt, act_nDigitInt, 8);
H_ACT_APPLY(act_tenDigitInt, act_nDigitInt, 10);

// Parser initialization function
void init_pacman() {
    /*  Whitespace  */
    H_RULE(ws, h_in((uint8_t*)" \r\n\t", 4));

    /*  Custom unsigned ints, due to how Hammer processes digits  */
    H_RULE(digit, h_ch_range(0x30, 0x39));

    H_ARULE(oneDigitInt, h_repeat_n(digit, 1));
    H_ARULE(twoDigitInt, h_choice(h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    H_ARULE(threeDigitInt, h_choice(h_repeat_n(digit, 3), h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    H_ARULE(fourDigitInt, h_choice(h_repeat_n(digit, 4), h_repeat_n(digit, 3), h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    H_ARULE(fiveDigitInt, h_choice(h_repeat_n(digit, 5), h_repeat_n(digit, 4), h_repeat_n(digit, 3), h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    H_ARULE(sevenDigitInt, h_choice(h_repeat_n(digit, 7), h_repeat_n(digit, 6), h_repeat_n(digit, 5), h_repeat_n(digit, 4), h_repeat_n(digit, 3), h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    H_ARULE(eightDigitInt, h_choice(h_repeat_n(digit, 8), h_repeat_n(digit, 7), h_repeat_n(digit, 6), h_repeat_n(digit, 5), h_repeat_n(digit, 4), h_repeat_n(digit, 3), h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    H_ARULE(tenDigitInt, h_choice(h_repeat_n(digit, 10), h_repeat_n(digit, 9), h_repeat_n(digit, 8), h_repeat_n(digit, 7), h_repeat_n(digit, 6), h_repeat_n(digit, 5), h_repeat_n(digit, 4), h_repeat_n(digit, 3), h_repeat_n(digit, 2), h_repeat_n(digit, 1), NULL));
    
    H_RULE(u1BitInt, h_int_range(oneDigitInt, 0, 1));
    H_RULE(u2BitInt, h_int_range(oneDigitInt, 0, 3));
    H_RULE(u3BitInt, h_int_range(oneDigitInt, 0, 7));
    H_RULE(u4BitInt, h_int_range(twoDigitInt, 0, 15));
    H_RULE(u5BitInt, h_int_range(twoDigitInt, 0, 31));
    H_RULE(u6BitInt, h_int_range(twoDigitInt, 0, 63));
    H_RULE(u7BitInt, h_int_range(threeDigitInt, 0, 127));
    H_RULE(u8BitInt, h_int_range(threeDigitInt, 0, 255));
    H_RULE(u9BitInt, h_int_range(threeDigitInt, 0, 511));
    H_RULE(u11BitInt, h_int_range(fourDigitInt, 0, 2047));
    H_RULE(u12BitInt, h_int_range(fourDigitInt, 0, 4095));
    H_RULE(u13BitInt, h_int_range(fourDigitInt, 0, 8191));
    H_RULE(u14BitInt, h_int_range(fiveDigitInt, 0, 16383));
    H_RULE(u15BitInt, h_int_range(fiveDigitInt, 0, 32767));
    H_RULE(u16BitInt, h_int_range(fiveDigitInt, 0, 65535));
    H_RULE(u20BitInt, h_int_range(sevenDigitInt, 0, 1048575));
    H_RULE(u23BitInt, h_int_range(sevenDigitInt, 0, 8388607));
    H_RULE(u24BitInt, h_int_range(eightDigitInt, 0, 16777215));
    H_RULE(u25BitInt, h_int_range(eightDigitInt, 0, 33554431));
    H_RULE(u32BitInt, h_int_range(tenDigitInt, 0, 4294967295));
    H_RULE(signed16BitInt, h_int_range(fiveDigitInt, 0, 32767));
    H_RULE(signed32BitInt, h_int_range(tenDigitInt, 0, 2147483647));

    H_RULE(preamble, h_sequence(h_ch('M'), 
                                h_ch('D'), 
                                h_ch('M'), 
                                NULL));
    H_RULE(ACK, h_choice(h_ch('0'), 
                         h_ch('1'), 
                         NULL));
    H_RULE(verMaj, u8BitInt);
    H_RULE(verMin, u8BitInt);
    H_RULE(version, h_sequence(verMaj,
                               ws, 
                               verMin, 
                               NULL));
    H_RULE(seqNum, u16BitInt);
    H_RULE(ML2BID, u32BitInt); // the first 16 bits is the message originator while the second 16 is the final recipient
    H_RULE(numMsgs, h_choice(h_ch('0'), 
                             h_ch('1'), 
                             NULL));


    /* TYPE 2 MESSAGES */
    H_RULE(typeFieldTwo, h_ch('2'));
    H_RULE(response, h_choice(h_token("16", 2),
                              h_token("32", 2), 
                              h_token("64", 2),
                              h_ch('1'), 
                              h_ch('2'), 
                              h_ch('4'), 
                              h_ch('8'), 
                              NULL)); // whitelist allowed values

    // insert the above into a Type 2 MDM
    H_RULE(typeTwo, h_sequence(preamble,
                               ws, 
                               response,
                               ws, 
                               version, 
                               ws,
                               seqNum, 
                               ws,
                               ML2BID,
                               ws, 
                               typeFieldTwo, 
                               ws,
                               numMsgs,            
                               h_end_p(),
                               NULL));
    /*   END TYPE 2    */

    /* TYPE 3 MESSAGES */
    H_RULE(typeFieldThree, h_ch('3'));
    H_RULE(timeOfDay, u32BitInt); // UTC time, but cannot be negative

    // insert the above into a Type 3 MDM
    H_RULE(typeThree, h_sequence(preamble,
                                 ws, 
                                 ACK, 
                                 ws,
                                 version, 
                                 ws,
                                 seqNum, 
                                 ws,
                                 ML2BID, 
                                 ws,
                                 typeFieldThree, 
                                 ws,
                                 numMsgs, 
                                 ws,
                                 timeOfDay, 
                                 h_end_p(),
                                 NULL));
    /*   END TYPE 3    */

    /* TYPE 4 MESSAGES */
    H_RULE(typeFieldFour, h_ch('4'));
    H_RULE(devType, h_int_range(u3BitInt, 0, 3));
    H_RULE(devID, u6BitInt);
    H_RULE(sigPortID, u7BitInt);
    H_RULE(sigPortResID, h_sequence(devType,
                                    ws, 
                                    devID, 
                                    ws,
                                    sigPortID, 
                                    NULL));
    H_RULE(UID, h_sequence(devType, 
                           ws,
                           devID, 
                           ws,
                           sigPortID, 
                           NULL));
    H_RULE(sigPortUID, h_sequence(sigPortResID, 
                                  ws,
                                  UID, 
                                  NULL));

    H_RULE(hexDigit, h_choice(h_ch_range('0', '9'),
                              h_ch_range('a', 'f'),
                              NULL));
    H_RULE(hexOctet, h_repeat_n(hexDigit, 2));

    H_RULE(userML2BCmdIPOne, h_choice(u32BitInt,
                                      h_repeat_n(hexOctet, 4),
                                      NULL));
    H_RULE(userML2BCmdIPTwo, h_choice(h_repeat_n(hexOctet, 4),
                                      h_ch('0'),
                                      NULL));
    H_RULE(userML2BCmdIPThree, h_choice(h_repeat_n(hexOctet, 4),
                                        h_ch('0'),
                                        NULL));
    H_RULE(userML2BCmdIPFour, h_choice(h_repeat_n(hexOctet, 4),
                                       h_ch('0'),
                                       NULL));
    H_RULE(userML2BCmdMACOne, h_repeat_n(hexOctet, 4));
    H_RULE(userML2BCmdMACTwo, h_repeat_n(hexOctet, 2));
    H_RULE(userML2BCmdUDP, u16BitInt);

    H_RULE(userML2BCmd, h_sequence(userML2BCmdIPOne,
                                   ws,
                                   userML2BCmdIPTwo, 
                                   ws,
                                   userML2BCmdIPThree, 
                                   ws,
                                   userML2BCmdIPFour, 
                                   ws,
                                   userML2BCmdMACOne, 
                                   ws,
                                   userML2BCmdMACTwo, 
                                   ws,
                                   userML2BCmdUDP, 
                                   NULL));

    H_RULE(userML2BSignalIPOne, h_choice(u32BitInt,
                                         h_repeat_n(hexOctet, 4),
                                         NULL));
    H_RULE(userML2BSignalIPTwo, h_choice(h_repeat_n(hexOctet, 4),
                                         h_ch('0'),
                                         NULL));
    H_RULE(userML2BSignalIPThree, h_choice(h_repeat_n(hexOctet, 4),
                                           h_ch('0'),
                                           NULL));
    H_RULE(userML2BSignalIPFour, h_choice(h_repeat_n(hexOctet, 4),
                                          h_ch('0'),
                                          NULL));
    H_RULE(userML2BSignalMACOne, h_repeat_n(hexOctet, 4));
    H_RULE(userML2BSignalMACTwo, h_repeat_n(hexOctet, 2));
    H_RULE(userML2BSignalUDP, u16BitInt);

    H_RULE(userML2BSignal, h_sequence(userML2BSignalIPOne,
                                      ws,
                                      userML2BSignalIPTwo,
                                      ws,
                                      userML2BSignalIPThree,
                                      ws,
                                      userML2BSignalIPFour,
                                      ws,
                                      userML2BSignalMACOne,
                                      ws,
                                      userML2BSignalMACTwo,
                                      ws,
                                      userML2BSignalUDP,
                                      NULL));

    H_RULE(userML2BContxIPOne, h_choice(u32BitInt,
                                        h_repeat_n(hexOctet, 4),
                                        NULL));
    H_RULE(userML2BContxIPTwo, h_choice(h_repeat_n(hexOctet, 4),
                                        h_ch('0'),
                                        NULL));
    H_RULE(userML2BContxIPThree, h_choice(h_repeat_n(hexOctet, 4),
                                          h_ch('0'),
                                          NULL));
    H_RULE(userML2BContxIPFour, h_choice(h_repeat_n(hexOctet, 4),
                                         h_ch('0'),
                                         NULL));
    H_RULE(userML2BContxMACOne, h_repeat_n(hexOctet, 4));
    H_RULE(userML2BContxMACTwo, h_repeat_n(hexOctet, 2));
    H_RULE(userML2BContxUDP, u16BitInt);

    H_RULE(userML2BContx, h_sequence(userML2BContxIPOne,
                                     ws,
                                     userML2BContxIPTwo, 
                                     ws,
                                     userML2BContxIPThree, 
                                     ws,
                                     userML2BContxIPFour, 
                                     ws,
                                     userML2BContxMACOne, 
                                     ws,
                                     userML2BContxMACTwo,
                                     ws, 
                                     userML2BContxUDP, 
                                     NULL));

    // insert the above into a Type 4 MDM
    H_RULE(typeFour, h_sequence(preamble,
                                ws, 
                                ACK,
                                ws, 
                                version,
                                ws, 
                                seqNum,
                                ws, 
                                ML2BID,
                                ws, 
                                typeFieldFour,
                                ws, 
                                numMsgs,
                                ws, 
                                sigPortUID,
                                ws, 
                                userML2BCmd,
                                ws, 
                                userML2BSignal,
                                ws, 
                                userML2BContx, 
                                h_end_p(),
                                NULL));
    /*   END TYPE 4    */

    /* TYPE 5 MESSAGES */
    H_RULE(typeFieldFive, h_ch('5'));
    H_RULE(alertType, u2BitInt);
    H_RULE(opParam, h_choice(h_token("16", 2),
                             h_token("32", 2), 
                             h_token("64", 2), 
                             h_token("128", 3), 
                             h_token("256", 3), 
                             h_token("512", 3),
                             h_token("1024", 4),
                             h_ch('0'),
                             h_ch('1'), 
                             h_ch('2'), 
                             h_ch('4'), 
                             h_ch('8'), 
                             NULL)); // whitelist allowed values
    H_RULE(healthStatusOne, h_sequence(alertType,
                                       ws,
                                       sigPortID,
                                       ws,
                                       opParam,
                                       NULL));

    H_RULE(opState, u16BitInt);
    H_RULE(paramCondition, h_choice(h_token("16", 2), 
                                    h_token("32", 2), 
                                    h_token("64", 2), 
                                    h_token("128", 3), 
                                    h_token("256", 3), 
                                    h_token("512", 3),
                                    h_token("1024", 4),
                                    h_token("2048", 4),
                                    h_ch('0'),
                                    h_ch('1'), 
                                    h_ch('2'), 
                                    h_ch('4'), 
                                    h_ch('8'), 
                                    NULL)); // whitelist allowed values
    H_RULE(healthStatusTwo, h_sequence(opState,
                                       ws,
                                       paramCondition,
                                       NULL));

    // insert the above into a Type 5 MDM
    H_RULE(typeFive, h_sequence(preamble,
                                ws,
                                ACK,
                                ws,
                                version,
                                ws,
                                seqNum,
                                ws,
                                ML2BID,
                                ws,
                                typeFieldFive,
                                ws,
                                numMsgs,
                                ws,
                                healthStatusOne,
                                ws,
                                healthStatusTwo,
                                h_end_p(),
                                NULL));
    /*   END TYPE 5    */

    /* TYPE 6 MESSAGES */
    H_RULE(typeFieldSix, h_ch('6'));
    H_RULE(cmdPortID, u7BitInt);
    H_RULE(command, h_choice(h_token("16", 2),
                             h_token("32", 2), 
                             h_token("64", 2), 
                             h_token("128", 3), 
                             h_token("256", 3), 
                             h_token("512", 3),
                             h_token("1024", 4),
                             h_ch('0'),
                             h_ch('1'), 
                             h_ch('2'), 
                             h_ch('4'), 
                             h_ch('8'), 
                             NULL)); // whitelist allowed values
    H_RULE(cmdField, h_sequence(cmdPortID,
                                ws,
                                command,
                                NULL));

    H_RULE(configField, u32BitInt);

    H_RULE(wfOpField, u32BitInt);

    // insert the above into a Type 6 MDM
    H_RULE(typeSix, h_sequence(preamble,
                               ws,
                               ACK,
                               ws,
                               version,
                               ws,
                               seqNum,
                               ws,
                               ML2BID,
                               ws,
                               typeFieldSix,
                               ws,
                               numMsgs,
                               ws,
                               cmdField,
                               ws,
                               configField,
                               ws,
                               wfOpField,
                               h_end_p(),
                               NULL));
    /*   END TYPE 6    */

    /* TYPE 7 MESSAGES */
    H_RULE(typeFieldSeven, h_ch('7'));
    H_RULE(switchID, u7BitInt);
    H_RULE(switchGrpResID, h_sequence(devType, 
                                      ws,
                                      devID, 
                                      ws,
                                      switchID, 
                                      NULL));

    H_RULE(switchGrpUID, h_sequence(switchGrpResID, 
                                    ws,
                                    UID, 
                                    NULL));

    // insert the above into a Type 7 MDM
    H_RULE(typeSeven, h_sequence(preamble,
                                 ws,
                                 ACK, 
                                 ws,
                                 version, 
                                 ws,
                                 seqNum, 
                                 ws,
                                 ML2BID, 
                                 ws,
                                 typeFieldSeven, 
                                 ws,
                                 numMsgs, 
                                 ws,
                                 switchGrpUID, 
                                 ws,
                                 userML2BCmd, 
                                 ws,
                                 userML2BContx, 
                                 h_end_p(),
                                 NULL));
    /*   END TYPE 7    */

    pacman = h_choice(typeTwo, 
                      typeThree, 
                      typeFour, 
                      typeFive, 
                      typeSix, 
                      typeSeven, 
                      NULL);
}

char* type_conv() {
    switch(TYPE) {
        case 1:
            return "type1";
        case 2:
            return "type2";
        case 3:
            return "type3";
        case 4:
            return "type4";
        case 5:
            return "type5";
        case 6:
            return "type6";
        case 7:
            return "type7";
    }
}

// Test suite could be moved to separate file in the future
static void test_MDM() {
    uint8_t *file_contents;
    size_t input_file_size;

    char typeStr[16];
    char* convStr = type_conv();
    strcpy(typeStr, convStr);
    strcat(typeStr, ".txt");
    char *tmp = strdup(typeStr);
    strcpy(typeStr, "./tests/");
    strcat(typeStr, tmp);

    FILE *input_file = fopen(typeStr, "r");
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    rewind(input_file);
    file_contents = malloc(input_file_size * sizeof(uint8_t));
    fread(file_contents, sizeof(uint8_t), input_file_size, input_file);
    fclose(input_file);

    g_check_parse_ok(pacman, PB_MIN, file_contents, input_file_size);
    TYPE++;
}

static void failing_MDM() {
    uint8_t *file_contents;
    size_t input_file_size;

    FILE *input_file = fopen("./tests/failMDM.txt", "r");
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    rewind(input_file);
    file_contents = malloc(input_file_size * sizeof(uint8_t));
    fread(file_contents, sizeof(uint8_t), input_file_size, input_file);
    fclose(input_file);

    g_check_parse_failed(pacman, PB_MIN, file_contents, input_file_size);
}

// Function to be used for inserting tests to assert against our parser
void register_parser_tests() {
    g_test_add_func("/ok/type2", test_MDM);
    g_test_add_func("/ok/type3", test_MDM);
    g_test_add_func("/ok/type4", test_MDM);
    g_test_add_func("/ok/type5", test_MDM);
    g_test_add_func("/ok/type6", test_MDM);
    g_test_add_func("/ok/type7", test_MDM);
    g_test_add_func("/failingMDM", failing_MDM);
}

int main(int argc, char *argv[]) {
    init_pacman();
    g_test_init(&argc, &argv, NULL);
    register_parser_tests();
    g_test_run();
}
