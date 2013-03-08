// ************************************************************
//                      ledcube.h
//  This contains most of the led cube defines including all
//  the LED color positions and layer defines
//

#define C1P1     0x0400
#define C1P2     0x0200
#define C1P3     0x0100
#define C1P4     0x2000
#define C1P5     0x1000
#define C1P6     0x0800
#define C1P7     0x0001
#define C1P8     0x8000
#define C1P9     0x4000
#define C1P10    0x0008
#define C1P11    0x0004
#define C1P12    0x0002
#define C1P13    0x0010
#define C1P14    0x0080
#define C1P15    0x0040
#define C1P16    0x0020

#define C2P1     C1P1
#define C2P2     C1P2
#define C2P3     C1P3
#define C2P4     C1P6
#define C2P5     C1P9
#define C2P6     C1P4
#define C2P7     C1P5  //pin 7
#define C2P8     C1P12 //pin 8
#define C2P9     C1P7  //pin 9
#define C2P10    C1P8  //pin 10
#define C2P11    C1P13 
#define C2P12    C1P10
#define C2P13    C1P11 
#define C2P14    C1P14 
#define C2P15    C1P15
#define C2P16    C1P16

#define C3P1     C1P15   // 1 = 5
#define C3P2     C1P2   // 2 = 2
#define C3P3     C1P5   // 3 = 15
#define C3P4     C1P6   // 4 = 8
#define C3P5     C1P1   // 5 = 3
#define C3P6     C1P8   // 6 = 4
#define C3P7     C1P9   // 7 = 11
#define C3P8     C1P4  // 8 = 6
#define C3P9     C1P11   // 9 = 7
#define C3P10    C1P12  //10 = 14
#define C3P11    C1P7  //11 = 9
#define C3P12    C1P16 //12 = 10
#define C3P13    C1P13  //13 = 13
#define C3P14    C1P10  //14 = 16
#define C3P15    C1P3  //15 = 1
#define C3P16    C1P14  //16 = 12

#define C4P1     C1P1   // 1 = 1
#define C4P2     C1P2   // 2 = 2
#define C4P3     C1P3   // 3 = 3
#define C4P4     C1P4   // 4 = 4
#define C4P5     C1P5   // 5 = 5
#define C4P6     C1P6   // 6 = 6
#define C4P7     C1P7   // 7 = 7
#define C4P8     C1P8  // 8 = 8
#define C4P9     C1P9   // 9 = 9
#define C4P10    C1P12  //10 = 11 blue 11
#define C4P11    C1P13  //11 = 12 nc 12
#define C4P12    C1P10 //12 = 10  Green 10
#define C4P13    C1P11  //13 = 14 red 13
#define C4P14    C1P14  //14 = 14
#define C4P15    C1P15  //15 = 15
#define C4P16    C1P16  //16 = 16

#define C5P1     C1P1  // 1 = 1
#define C5P2     C1P2   // 2 = 2
#define C5P3     C1P3   // 3 = 3
#define C5P4     C1P4   // 4 = 4
#define C5P5     C1P5   // 5 = 5
#define C5P6     C1P6   // 6 = 6
#define C5P7     C1P7   // 7 = 7
#define C5P8     C1P8   // 8 = 8
#define C5P9     C1P9  // 9 = 9
#define C5P10    C1P10  //10 = 10
#define C5P11    C1P11  //11 = 11
#define C5P12    C1P12  //12 = 12
#define C5P13    C1P15  //13 = 15
#define C5P14    C1P16  //14 = 16
#define C5P15    C1P13   //15 = 13
#define C5P16    C1P14  //16 = 14

#define R1 (C2P8>>8)


uint16_t R[25][5] PROGMEM = {   {0    ,C2P8 ,0    ,0    ,0    }, //1
                        {0    ,C2P7 ,0    ,0    ,0    }, //2
                        {0    ,0    ,C3P15,0    ,0    }, //3
                        {0    ,0    ,C3P14,0    ,0    }, //4
                        {0    ,0    ,C3P9 ,0    ,0    }, //5
                        {0    ,C2P11,0    ,0    ,0    }, //6
                        {0    ,C2P3 ,0    ,0    ,0    }, //7
                        {0    ,0    ,C3P3 ,0    ,0    }, //8
                        {0    ,0    ,C3P8 ,0    ,0    }, //9
                        {0    ,0    ,0    ,C4P7 ,0    }, //10
                        {0    ,C2P14,0    ,0    ,0    },
                        {C1P6 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P11,0    },
                        {0    ,0    ,0    ,C4P6 ,0    },
                        {0    ,0    ,0    ,C4P3 ,0    }, //15
                        {C1P7 ,0    ,0    ,0    ,0    },
                        {C1P3 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P14,0    },
                        {0    ,0    ,0    ,0    ,C5P13},
                        {0    ,0    ,0    ,0    ,C5P12}, //20
                        {C1P10,0    ,0    ,0    ,0    },
                        {C1P16,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,0    ,C5P3},
                        {0    ,0    ,0    ,0    ,C5P4},
                        {0    ,0    ,0    ,0    ,C5P9} //25
};
uint16_t G[25][5] PROGMEM = {   {0    ,C2P9 ,0    ,0    ,0    }, //1
                        {0    ,C2P6 ,0    ,0    ,0    }, //2
                        {0    ,0    ,C3P16,0    ,0    }, //3
                        {0    ,0    ,C3P13,0    ,0    }, //4
                        {0    ,0    ,C3P10,0    ,0    }, //5
                        {0    ,C2P12,0    ,0    ,0    }, //6
                        {0    ,C2P2 ,0    ,0    ,0    }, //7
                        {0    ,0    ,C3P4 ,0    ,0    }, //8
                        {0    ,0    ,C3P7 ,0    ,0    }, //9
                        {0    ,0    ,0    ,C4P8 ,0    }, //10
                        {0    ,C2P15,0    ,0    ,0    },
                        {C1P5 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P12,0    },
                        {0    ,0    ,0    ,C4P5 ,0    },
                        {0    ,0    ,0    ,C4P2 ,0    }, //15
                        {C1P8 ,0    ,0    ,0    ,0    },
                        {C1P2 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P15,0    },
                        {0    ,0    ,0    ,0    ,C5P14},
                        {0    ,0    ,0    ,0    ,C5P11}, //20
                        {C1P11,0    ,0    ,0    ,0    },
                        {C1P15,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,0    ,C5P2 },
                        {0    ,0    ,0    ,0    ,C5P5 },
                        {0    ,0    ,0    ,0    ,C5P8 } //25
};
uint16_t B[25][5] PROGMEM = {   {0    ,C2P10,0    ,0    ,0    }, //1
                        {0    ,C2P5 ,0    ,0    ,0    }, //2
                        {0    ,0    ,C3P1 ,0    ,0    }, //3
                        {0    ,0    ,C3P12,0    ,0    }, //4
                        {0    ,0    ,C3P11,0    ,0    }, //5
                        {0    ,C2P13,0    ,0    ,0    }, //6
                        {0    ,C2P1 ,0    ,0    ,0    }, //7
                        {0    ,0    ,C3P5 ,0    ,0    }, //8
                        {0    ,0    ,C3P6 ,0    ,0    }, //9
                        {0    ,0    ,0    ,C4P9 ,0    }, //10
                        {0    ,C2P16,0    ,0    ,0    },
                        {C1P4 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P13,0    },
                        {0    ,0    ,0    ,C4P4 ,0    },
                        {0    ,0    ,0    ,C4P1 ,0    }, //15
                        {C1P9 ,0    ,0    ,0    ,0    },
                        {C1P1 ,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,C4P16,0    },
                        {0    ,0    ,0    ,0    ,C5P15},
                        {0    ,0    ,0    ,0    ,C5P10}, //20
                        {C1P12,0    ,0    ,0    ,0    },
                        {C1P14,0    ,0    ,0    ,0    },
                        {0    ,0    ,0    ,0    ,C5P1 },
                        {0    ,0    ,0    ,0    ,C5P6 },
                        {0    ,0    ,0    ,0    ,C5P7 } //25
};

uint32_t power_duration[3] PROGMEM = {0x1FFFFFF,0x1FFFFFF,0x1FFFFFF};
uint32_t off[5][3] PROGMEM = {
                {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
                };
uint32_t on[5][3] PROGMEM = 
    {
    {0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF}
    };
uint32_t pattern1[5][5][3] PROGMEM = 
    {
    {{0x1FFFFFF,0,0},{0x1FFFFFF,0,0},{0x1FFFFFF,0,0},{0x1FFFFFF,0,0},{0x1FFFFFF,0,0}},
    {{0,0x1FFFFFF,0},{0,0x1FFFFFF,0},{0,0x1FFFFFF,0},{0,0x1FFFFFF,0},{0,0x1FFFFFF,0}},
    {{0,0,0x1FFFFFF},{0,0,0x1FFFFFF},{0,0,0x1FFFFFF},{0,0,0x1FFFFFF},{0,0,0x1FFFFFF}},
    {{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF},{0x1FFFFFF,0x1FFFFFF,0x1FFFFFF}},
    {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0,}}
    };
uint32_t pattern2[5][5][3] PROGMEM = 
    {
    {{0x1,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
    {{0x2,0,0},{0,0,0},{0,0,},{0,0,0},{0,0,0}},
    {{0x4,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
    {{0x8,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
    {{0x10,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0,}}
    };

//This is the magnet test
//Displays red column over each hall effect sensor
uint32_t pattern3[5][5][3] PROGMEM = 
    {
    {{0x4,0,0},{0x4,0,0},{0x4,0,0},{0x4,0,0},{0x4,0,0}},
    {{0x400,0,0},{0x400,0,0},{0x400,0,},{0x400,0,0},{0x400,0,0}},
    {{0x4000,0},{0x4000,0,0},{0x4000,0,0},{0x4000,0,0},{0x4000,0,0}},
    {{0x400000,0},{0x400000,0,0},{0x400000,0,0},{0x400000,0,0},{0x400000,0,0}},
    {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0,}}
    };

