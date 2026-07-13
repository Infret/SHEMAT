#include "bindingenergydata.h"

BindingEnergyEntry BindingEnergyData::get(int A, int Z) const {
    return bindingData.value(qMakePair(A, Z), BindingEnergyEntry{0, 0, 0, "", 0.0, 0.0});
}

double BindingEnergyData::getNeutronBindingEnergy(int A, int Z) const {
    return get(A, Z).neutronBindingEnergy;
}

double BindingEnergyData::getProtonBindingEnergy(int A, int Z) const {
    return get(A, Z).protonBindingEnergy;
}

BindingEnergyData::BindingEnergyData() {
    // A=181
    bindingData[qMakePair(181, 69)] = {112, 69, 181, "Tm", 5320.0, 0.0};
    bindingData[qMakePair(181, 70)] = {111, 70, 181, "Yb", 4560.0, 10460.0};
    bindingData[qMakePair(181, 71)] = {110, 71, 181, "Lu", 6190.0, 7490.0};
    bindingData[qMakePair(181, 72)] = {109, 72, 181, "Hf", 5694.80, 8020.0};
    bindingData[qMakePair(181, 73)] = {108, 73, 181, "Ta", 7576.8, 5947.9};
    bindingData[qMakePair(181, 74)] = {107, 74, 181, "W", 6669.02, 6589.9};
    bindingData[qMakePair(181, 75)] = {106, 75, 181, "Re", 8751.0, 4170.0};
    bindingData[qMakePair(181, 76)] = {105, 76, 181, "Os", 7260.0, 5000.0};
    bindingData[qMakePair(181, 77)] = {104, 77, 181, "Ir", 9557.0, 2394.0};
    bindingData[qMakePair(181, 78)] = {103, 78, 181, "Pt", 8017.0, 3693.0};
    bindingData[qMakePair(181, 79)] = {102, 79, 181, "Au", 10317.0, 724.0};
    bindingData[qMakePair(181, 80)] = {101, 80, 181, "Hg", 8482.0, 2325.0};
    bindingData[qMakePair(181, 81)] = {100, 81, 181, "Tl", 11480.0, -163.0};
    bindingData[qMakePair(181, 82)] = {99, 82, 181, "Pb", 9250.0, 1020.0};

    // A=182
    bindingData[qMakePair(182, 70)] = {112, 70, 182, "Yb", 5800.0, 10940.0};
    bindingData[qMakePair(182, 71)] = {111, 71, 182, "Lu", 5150.0, 8080.0};
    bindingData[qMakePair(182, 72)] = {110, 72, 182, "Hf", 6718.0, 8540.0};
    bindingData[qMakePair(182, 73)] = {109, 73, 182, "Ta", 6062.94, 6316.1};
    bindingData[qMakePair(182, 74)] = {108, 74, 182, "W", 8083.6, 7096.7};
    bindingData[qMakePair(182, 75)] = {107, 75, 182, "Re", 7000.0, 4500.0};
    bindingData[qMakePair(182, 76)] = {106, 76, 182, "Os", 9130.0, 5381.0};
    bindingData[qMakePair(182, 77)] = {105, 77, 182, "Ir", 7660.0, 2790.0};
    bindingData[qMakePair(182, 78)] = {104, 78, 182, "Pt", 9858.0, 3994.0};
    bindingData[qMakePair(182, 79)] = {103, 79, 182, "Au", 8501.0, 1208.0};
    bindingData[qMakePair(182, 80)] = {102, 80, 182, "Hg", 10987.0, 2995.0};
    bindingData[qMakePair(182, 81)] = {101, 81, 182, "Tl", 8601.0, 0.0};        // S(p)=-44
    bindingData[qMakePair(182, 82)] = {100, 82, 182, "Pb", 11780.0, 1315.0};

    // A=183
    bindingData[qMakePair(183, 70)] = {113, 70, 183, "Yb", 4350.0, 0.0};        // S(p)=*
    bindingData[qMakePair(183, 71)] = {112, 71, 183, "Lu", 5910.0, 8190.0};
    bindingData[qMakePair(183, 72)] = {111, 72, 183, "Hf", 5300.0, 8690.0};
    bindingData[qMakePair(183, 73)] = {110, 73, 183, "Ta", 6934.18, 6532.0};
    bindingData[qMakePair(183, 74)] = {109, 74, 183, "W", 6190.84, 7224.6};
    bindingData[qMakePair(183, 75)] = {108, 75, 183, "Re", 8430.0, 4852.0};
    bindingData[qMakePair(183, 76)] = {107, 76, 183, "Os", 7130.0, 5510.0};
    bindingData[qMakePair(183, 77)] = {106, 77, 183, "Ir", 9220.0, 2880.0};
    bindingData[qMakePair(183, 78)] = {105, 78, 183, "Pt", 7675.0, 4010.0};
    bindingData[qMakePair(183, 79)] = {104, 79, 183, "Au", 9962.0, 1312.0};
    bindingData[qMakePair(183, 80)] = {103, 80, 183, "Hg", 8299.0, 2793.0};
    bindingData[qMakePair(183, 81)] = {102, 81, 183, "Tl", 11331.0, 299.0};
    bindingData[qMakePair(183, 82)] = {101, 82, 183, "Pb", 8820.0, 1540.0};

    // A=184
    bindingData[qMakePair(184, 70)] = {114, 70, 184, "Yb", 5510.0, 0.0};
    bindingData[qMakePair(184, 71)] = {113, 71, 184, "Lu", 4770.0, 8600.0};
    bindingData[qMakePair(184, 72)] = {112, 72, 184, "Hf", 6290.0, 9070.0};
    bindingData[qMakePair(184, 73)] = {111, 73, 184, "Ta", 5618.0, 6850.0};
    bindingData[qMakePair(184, 74)] = {110, 74, 184, "W", 7411.11, 7701.5};
    bindingData[qMakePair(184, 75)] = {109, 75, 184, "Re", 6481.0, 5143.0};
    bindingData[qMakePair(184, 76)] = {108, 76, 184, "Os", 8660.0, 5732.0};
    bindingData[qMakePair(184, 77)] = {107, 77, 184, "Ir", 7480.0, 3240.0};
    bindingData[qMakePair(184, 78)] = {106, 78, 184, "Pt", 9633.0, 4420.0};
    bindingData[qMakePair(184, 79)] = {105, 79, 184, "Au", 8199.0, 1835.0};
    bindingData[qMakePair(184, 80)] = {104, 80, 184, "Hg", 10616.0, 3446.0};
    bindingData[qMakePair(184, 81)] = {103, 81, 184, "Tl", 8367.0, 368.0};
    bindingData[qMakePair(184, 82)] = {103, 81, 184, "Pb", 11550.0, 1753.0};
    bindingData[qMakePair(184, 83)] = {103, 81, 184, "Bi", 0.0, 0.0};      // S(p)=−1350

    // A=185
       bindingData[qMakePair(185, 70)] = {115, 70, 185, "Yb", 4030.0, 0.0};        // S(p)=*
       bindingData[qMakePair(185, 71)] = {114, 71, 185, "Lu", 5550.0, 8640.0};
       bindingData[qMakePair(185, 72)] = {113, 72, 185, "Hf", 4890.0, 9200.0};
       bindingData[qMakePair(185, 73)] = {112, 73, 185, "Ta", 6626.0, 7180.0};
       bindingData[qMakePair(185, 74)] = {111, 74, 185, "W", 5753.74, 7837.0};
       bindingData[qMakePair(185, 75)] = {110, 75, 185, "Re", 7671.0, 5402.6};
       bindingData[qMakePair(185, 76)] = {109, 76, 185, "Os", 6624.66, 5875.0};
       bindingData[qMakePair(185, 77)] = {108, 77, 185, "Ir", 8800.0, 3372.0};
       bindingData[qMakePair(185, 78)] = {107, 78, 185, "Pt", 7430.0, 4370.0};
       bindingData[qMakePair(185, 79)] = {106, 79, 185, "Au", 9611.0, 1813.0};
       bindingData[qMakePair(185, 80)] = {105, 80, 185, "Hg", 7906.0, 3154.0};
       bindingData[qMakePair(185, 81)] = {104, 81, 185, "Tl", 10946.0, 698.0};
       bindingData[qMakePair(185, 82)] = {103, 82, 185, "Pb", 8551.0, 1947.0};
       bindingData[qMakePair(185, 83)] = {102, 83, 185, "Bi", 11370.0, -1530.0};   // S(p)=-1530#

       // A=186
       bindingData[qMakePair(186, 71)] = {115, 71, 186, "Lu", 4390.0, 9000.0};
       bindingData[qMakePair(186, 72)] = {114, 72, 186, "Hf", 6180.0, 9830.0};
       bindingData[qMakePair(186, 73)] = {113, 73, 186, "Ta", 5280.0, 7580.0};
       bindingData[qMakePair(186, 74)] = {112, 74, 186, "W", 7192.1, 8403.0};
       bindingData[qMakePair(186, 75)] = {111, 75, 186, "Re", 6179.38, 5828.3};
       bindingData[qMakePair(186, 76)] = {110, 76, 186, "Os", 8265.4, 6469.9};
       bindingData[qMakePair(186, 77)] = {109, 77, 186, "Ir", 6910.0, 3655.0};
       bindingData[qMakePair(186, 78)] = {108, 78, 186, "Pt", 9250.0, 4820.0};
       bindingData[qMakePair(186, 79)] = {107, 79, 186, "Au", 7928.0, 2320.0};
       bindingData[qMakePair(186, 80)] = {106, 80, 186, "Hg", 10427.0, 3970.0};
       bindingData[qMakePair(186, 81)] = {105, 81, 186, "Tl", 8200.0, 992.0};
       bindingData[qMakePair(186, 82)] = {104, 82, 186, "Pb", 11212.0, 2213.0};
       bindingData[qMakePair(186, 83)] = {103, 83, 186, "Bi", 8980.0, -1106.0};    // S(p)=-1106
       bindingData[qMakePair(186, 84)] = {102, 84, 186, "Po", 0.0, 950.0};

       // A=187
       bindingData[qMakePair(187, 71)] = {116, 71, 187, "Lu", 5440.0, 0.0};        // S(p)=*
       bindingData[qMakePair(187, 72)] = {115, 72, 187, "Hf", 4460.0, 9900.0};
       bindingData[qMakePair(187, 73)] = {114, 73, 187, "Ta", 6360.0, 7760.0};
       bindingData[qMakePair(187, 74)] = {113, 74, 187, "W", 5466.76, 8590.0};
       bindingData[qMakePair(187, 75)] = {112, 75, 187, "Re", 7360.7, 5996.9};
       bindingData[qMakePair(187, 76)] = {111, 76, 187, "Os", 6290.3, 6580.8};
       bindingData[qMakePair(187, 77)] = {110, 77, 187, "Ir", 8450.0, 3838.0};
       bindingData[qMakePair(187, 78)] = {109, 78, 187, "Pt", 6890.0, 4802.0};
       bindingData[qMakePair(187, 79)] = {108, 79, 187, "Au", 9380.0, 2450.0};
       bindingData[qMakePair(187, 80)] = {107, 80, 187, "Hg", 7650.0, 3692.0};
       bindingData[qMakePair(187, 81)] = {106, 81, 187, "Tl", 10629.0, 1194.0};
       bindingData[qMakePair(187, 82)] = {105, 82, 187, "Pb", 8376.0, 2389.0};
       bindingData[qMakePair(187, 83)] = {104, 83, 187, "Bi", 11308.0, -1010.0};   // S(p)=-1010
       bindingData[qMakePair(187, 84)] = {103, 84, 187, "Po", 9340.0, 1310.0};     // S(p)=1310

       // A=188
       bindingData[qMakePair(188, 71)] = {117, 71, 188, "Lu", 4280.0, 0.0};        // S(p)=*
       bindingData[qMakePair(188, 72)] = {116, 72, 188, "Hf", 6130.0, 10590.0};
       bindingData[qMakePair(188, 73)] = {115, 73, 188, "Ta", 4790.0, 8080.0};
       bindingData[qMakePair(188, 74)] = {114, 74, 188, "W", 6835.0, 9060.0};
       bindingData[qMakePair(188, 75)] = {113, 75, 188, "Re", 5871.65, 6401.8};
       bindingData[qMakePair(188, 76)] = {112, 76, 188, "Os", 7899.61, 7209.73};
       bindingData[qMakePair(188, 77)] = {111, 77, 188, "Ir", 6867.0, 4415.0};
       bindingData[qMakePair(188, 78)] = {110, 78, 188, "Pt", 9207.0, 5561.0};
       bindingData[qMakePair(188, 79)] = {109, 79, 188, "Au", 7415.0, 2975.0};
       bindingData[qMakePair(188, 80)] = {108, 80, 188, "Hg", 10155.0, 4463.0};
       bindingData[qMakePair(188, 81)] = {107, 81, 188, "Tl", 7960.0, 1510.0};
       bindingData[qMakePair(188, 82)] = {106, 82, 188, "Pb", 10900.0, 2660.0};
       bindingData[qMakePair(188, 83)] = {105, 83, 188, "Bi", 8883.0, -503.0};     // S(p)=-503
       bindingData[qMakePair(188, 84)] = {104, 84, 188, "Po", 11440.0, 1450.0};
       // A=189
       bindingData[qMakePair(189, 72)] = {117, 72, 189, "Hf", 4360.0, 10660.0};
       bindingData[qMakePair(189, 73)] = {116, 73, 189, "Ta", 6290.0, 8240.0};
       bindingData[qMakePair(189, 74)] = {115, 74, 189, "W", 5020.0, 9290.0};
       bindingData[qMakePair(189, 75)] = {114, 75, 189, "Re", 7034.0, 6600.0};
       bindingData[qMakePair(189, 76)] = {113, 76, 189, "Os", 5920.8, 7258.9};
       bindingData[qMakePair(189, 77)] = {112, 77, 189, "Ir", 8176.0, 4601.0};
       bindingData[qMakePair(189, 78)] = {111, 78, 189, "Pt", 6720.0, 5413.0};
       bindingData[qMakePair(189, 79)] = {110, 79, 189, "Au", 9282.0, 3050.0};
       bindingData[qMakePair(189, 80)] = {109, 80, 189, "Hg", 7500.0, 4540.0};
       bindingData[qMakePair(189, 81)] = {108, 81, 189, "Tl", 10350.0, 1703.0};
       bindingData[qMakePair(189, 82)] = {107, 82, 189, "Pb", 8100.0, 2800.0};
       bindingData[qMakePair(189, 83)] = {106, 83, 189, "Bi", 10941.0, -462.0};
       bindingData[qMakePair(189, 84)] = {105, 84, 189, "Po", 8949.0, 1516.0};

        // A=190
        bindingData[qMakePair(190, 72)] = {118, 72, 190, "Hf", 5940.0, 0.0};        // S(p)=*
        bindingData[qMakePair(190, 73)] = {117, 73, 190, "Ta", 4760.0, 8640.0};
        bindingData[qMakePair(190, 74)] = {116, 74, 190, "W", 6840.0, 9840.0};
        bindingData[qMakePair(190, 75)] = {115, 75, 190, "Re", 5730.0, 7310.0};
        bindingData[qMakePair(190, 76)] = {114, 76, 190, "Os", 7792.34, 8018.0};
        bindingData[qMakePair(190, 77)] = {113, 77, 190, "Ir", 6375.0, 5055.8};
        bindingData[qMakePair(190, 78)] = {112, 78, 190, "Pt", 8908.0, 6146.0};
        bindingData[qMakePair(190, 79)] = {111, 79, 190, "Au", 7323.0, 3653.0};
        bindingData[qMakePair(190, 80)] = {110, 80, 190, "Hg", 9820.0, 5078.0};
        bindingData[qMakePair(190, 81)] = {109, 81, 190, "Tl", 7827.0, 2030.0};
        bindingData[qMakePair(190, 82)] = {108, 82, 190, "Pb", 10644.0, 3090.0};
        bindingData[qMakePair(190, 83)] = {107, 83, 190, "Bi", 8610.0, 45.0};
        bindingData[qMakePair(190, 84)] = {106, 84, 190, "Po", 11213.0, 1788.0};

        // A=191
        bindingData[qMakePair(191, 73)] = {118, 73, 191, "Ta", 6050.0, 8750.0};
        bindingData[qMakePair(191, 74)] = {117, 74, 191, "W", 4870.0, 9950.0};
        bindingData[qMakePair(191, 75)] = {116, 75, 191, "Re", 6790.0, 7260.0};
        bindingData[qMakePair(191, 76)] = {115, 76, 191, "Os", 5758.73, 8050.0};
        bindingData[qMakePair(191, 77)] = {114, 77, 191, "Ir", 8026.5, 5290.0};
        bindingData[qMakePair(191, 78)] = {113, 78, 191, "Pt", 6463.0, 6234.0};
        bindingData[qMakePair(191, 79)] = {112, 79, 191, "Au", 9030.0, 3780.0};
        bindingData[qMakePair(191, 80)] = {111, 80, 191, "Hg", 7293.0, 5047.0};
        bindingData[qMakePair(191, 81)] = {110, 81, 191, "Tl", 9982.0, 2201.0};
        bindingData[qMakePair(191, 82)] = {109, 82, 191, "Pb", 7890.0, 3150.0};
        bindingData[qMakePair(191, 83)] = {108, 83, 191, "Bi", 10711.0, 112.0};
        bindingData[qMakePair(191, 84)] = {107, 84, 191, "Po", 8576.0, 1758.0};
        bindingData[qMakePair(191, 85)] = {106, 85, 191, "At", 0.0, -1139.0};       // S(n)=*

        // A=192
        bindingData[qMakePair(192, 73)] = {119, 73, 192, "Ta", 4640.0, 0.0};        // S(p)=*
        bindingData[qMakePair(192, 74)] = {118, 74, 192, "W", 6550.0, 10450.0};
        bindingData[qMakePair(192, 75)] = {117, 75, 192, "Re", 5310.0, 7700.0};
        bindingData[qMakePair(192, 76)] = {116, 76, 192, "Os", 7558.3, 8821.0};
        bindingData[qMakePair(192, 77)] = {115, 77, 192, "Ir", 6198.12, 5729.3};
        bindingData[qMakePair(192, 78)] = {114, 78, 192, "Pt", 8661.5, 6868.7};
        bindingData[qMakePair(192, 79)] = {113, 79, 192, "Au", 7046.0, 4363.0};
        bindingData[qMakePair(192, 80)] = {112, 80, 192, "Hg", 9491.0, 5503.0};
        bindingData[qMakePair(192, 81)] = {111, 81, 192, "Tl", 7660.0, 2570.0};
        bindingData[qMakePair(192, 82)] = {110, 82, 192, "Pb", 10400.0, 3562.0};
        bindingData[qMakePair(192, 83)] = {109, 83, 192, "Bi", 8370.0, 590.0};
        bindingData[qMakePair(192, 84)] = {108, 84, 192, "Po", 11073.0, 2120.0};
        bindingData[qMakePair(192, 85)] = {107, 85, 192, "At", 9010.0, -706.0};

            // A=193
            bindingData[qMakePair(193, 73)] = {120, 73, 193, "Ta", 5880.0, 0.0};       // S(p)=*
            bindingData[qMakePair(193, 74)] = {119, 74, 193, "W", 4710.0, 10510.0};
            bindingData[qMakePair(193, 75)] = {118, 75, 193, "Re", 6710.0, 7870.0};
            bindingData[qMakePair(193, 76)] = {117, 76, 193, "Os", 5583.42, 9090.0};
            bindingData[qMakePair(193, 77)] = {116, 77, 193, "Ir", 7771.99, 5943.0};
            bindingData[qMakePair(193, 78)] = {115, 78, 193, "Pt", 6262.5, 6933.0};
            bindingData[qMakePair(193, 79)] = {114, 79, 193, "Au", 8704.0, 4405.0};
            bindingData[qMakePair(193, 80)] = {113, 80, 193, "Hg", 7122.0, 5579.0};
            bindingData[qMakePair(193, 81)] = {112, 81, 193, "Tl", 9680.0, 2755.0};
            bindingData[qMakePair(193, 82)] = {111, 82, 193, "Pb", 7710.0, 3610.0};
            bindingData[qMakePair(193, 83)] = {110, 83, 193, "Bi", 10420.0, 618.0};
            bindingData[qMakePair(193, 84)] = {109, 84, 193, "Po", 8326.0, 2080.0};
            bindingData[qMakePair(193, 85)] = {108, 85, 193, "At", 11060.0, -714.0};
            bindingData[qMakePair(193, 86)] = {107, 86, 193, "Rn", 0.0, 1170.0};       // S(n)=*

            // A=194
            bindingData[qMakePair(194, 73)] = {121, 73, 194, "Ta", 4500.0, 0.0};       // S(p)=*
            bindingData[qMakePair(194, 74)] = {120, 74, 194, "W", 6310.0, 10950.0};
            bindingData[qMakePair(194, 75)] = {119, 75, 194, "Re", 5080.0, 8240.0};
            bindingData[qMakePair(194, 76)] = {118, 76, 194, "Os", 7112.0, 9490.0};
            bindingData[qMakePair(194, 77)] = {117, 77, 194, "Ir", 6066.79, 6426.4};
            bindingData[qMakePair(194, 78)] = {116, 78, 194, "Pt", 8351.8, 7512.8};
            bindingData[qMakePair(194, 79)] = {115, 79, 194, "Au", 6878.0, 5021.3};
            bindingData[qMakePair(194, 80)] = {114, 80, 194, "Hg", 9193.0, 6068.0};
            bindingData[qMakePair(194, 81)] = {113, 81, 194, "Tl", 7532.0, 3164.0};
            bindingData[qMakePair(194, 82)] = {112, 82, 194, "Pb", 10080.0, 4020.0};
            bindingData[qMakePair(194, 83)] = {111, 83, 194, "Bi", 8216.0, 1120.0};
            bindingData[qMakePair(194, 84)] = {110, 84, 194, "Po", 10751.0, 2409.0};
            bindingData[qMakePair(194, 85)] = {109, 85, 194, "At", 8720.0, -316.0};
            bindingData[qMakePair(194, 86)] = {108, 86, 194, "Rn", 11390.0, 1498.0};

            // A=195
            bindingData[qMakePair(195, 74)] = {121, 74, 195, "W", 4560.0, 11000.0};
            bindingData[qMakePair(195, 75)] = {120, 75, 195, "Re", 6410.0, 8340.0};
            bindingData[qMakePair(195, 76)] = {119, 76, 195, "Os", 5150.0, 9560.0};
            bindingData[qMakePair(195, 77)] = {118, 77, 195, "Ir", 7231.86, 6546.1};
            bindingData[qMakePair(195, 78)] = {117, 78, 195, "Pt", 6105.10, 7551.1};
            bindingData[qMakePair(195, 79)] = {116, 79, 195, "Au", 8426.4, 5095.9};
            bindingData[qMakePair(195, 80)] = {115, 80, 195, "Hg", 6901.0, 6090.0};
            bindingData[qMakePair(195, 81)] = {114, 81, 195, "Tl", 9289.0, 3260.0};
            bindingData[qMakePair(195, 82)] = {113, 82, 195, "Pb", 7571.0, 4095.0};
            bindingData[qMakePair(195, 83)] = {112, 83, 195, "Bi", 10068.0, 1107.0};
            bindingData[qMakePair(195, 84)] = {111, 84, 195, "Po", 8120.0, 2320.0};
            bindingData[qMakePair(195, 85)] = {110, 85, 195, "At", 10821.0, -245.0};
            bindingData[qMakePair(195, 86)] = {109, 86, 195, "Rn", 8740.0, 1520.0};

            // A=196
            bindingData[qMakePair(196, 74)] = {122, 74, 196, "W", 5940.0, 0.0};        // S(p)=*
            bindingData[qMakePair(196, 75)] = {121, 75, 196, "Re", 5040.0, 8820.0};    // S(p)=8820#
            bindingData[qMakePair(196, 76)] = {120, 76, 196, "Os", 6840.0, 9990.0};    // S(p)=9990#
            bindingData[qMakePair(196, 77)] = {119, 77, 196, "Ir", 5810.0, 7210.0};
            bindingData[qMakePair(196, 78)] = {118, 78, 196, "Pt", 7921.98, 8241.2};
            bindingData[qMakePair(196, 79)] = {117, 79, 196, "Au", 6643.0, 5633.8};
            bindingData[qMakePair(196, 80)] = {116, 80, 196, "Hg", 8884.0, 6548.0};
            bindingData[qMakePair(196, 81)] = {115, 81, 196, "Tl", 7413.0, 3772.0};
            bindingData[qMakePair(196, 82)] = {114, 82, 196, "Pb", 9712.0, 4482.0};
            bindingData[qMakePair(196, 83)] = {113, 83, 196, "Bi", 8055.0, 1590.0};
            bindingData[qMakePair(196, 84)] = {112, 84, 196, "Po", 10490.0, 2756.0};
            bindingData[qMakePair(196, 85)] = {111, 85, 196, "At", 8520.0, 150.0};
            bindingData[qMakePair(196, 86)] = {110, 86, 196, "Rn", 11150.0, 1848.0};

                // A=197
                bindingData[qMakePair(197, 74)] = {123, 74, 197, "W", 4330.0, 0.0};       // S(p)=*
                bindingData[qMakePair(197, 75)] = {122, 75, 197, "Re", 6030.0, 8910.0};
                bindingData[qMakePair(197, 76)] = {121, 76, 197, "Os", 5100.0, 10060.0};
                bindingData[qMakePair(197, 77)] = {120, 77, 197, "Ir", 6900.0, 7280.0};
                bindingData[qMakePair(197, 78)] = {119, 78, 197, "Pt", 5846.56, 8270.0};
                bindingData[qMakePair(197, 79)] = {118, 79, 197, "Au", 8072.3, 5784.2};
                bindingData[qMakePair(197, 80)] = {117, 80, 197, "Hg", 6785.6, 6690.0};
                bindingData[qMakePair(197, 81)] = {116, 81, 197, "Tl", 8916.0, 3805.0};
                bindingData[qMakePair(197, 82)] = {115, 82, 197, "Pb", 7468.0, 4538.0};
                bindingData[qMakePair(197, 83)] = {114, 83, 197, "Bi", 9749.0, 1628.0};
                bindingData[qMakePair(197, 84)] = {113, 84, 197, "Po", 7960.0, 2640.0};
                bindingData[qMakePair(197, 85)] = {112, 85, 197, "At", 10510.0, 171.0};
                bindingData[qMakePair(197, 86)] = {111, 86, 197, "Rn", 8532.0, 1860.0};
                bindingData[qMakePair(197, 87)] = {110, 87, 197, "Fr", 0.0, -990.0};       // S(n)=*

                // A=198
                bindingData[qMakePair(198, 75)] = {123, 75, 198, "Re", 4710.0, 9290.0};
                bindingData[qMakePair(198, 76)] = {122, 76, 198, "Os", 6600.0, 10620.0};
                bindingData[qMakePair(198, 77)] = {121, 77, 198, "Ir", 5650.0, 7800.0};
                bindingData[qMakePair(198, 78)] = {120, 78, 198, "Pt", 7555.6, 8929.0};
                bindingData[qMakePair(198, 79)] = {119, 79, 198, "Au", 6512.36, 6450.0};
                bindingData[qMakePair(198, 80)] = {118, 80, 198, "Hg", 8485.0, 7103.5};
                bindingData[qMakePair(198, 81)] = {117, 81, 198, "Tl", 7258.0, 4277.0};
                bindingData[qMakePair(198, 82)] = {116, 82, 198, "Pb", 9393.0, 5015.0};
                bindingData[qMakePair(198, 83)] = {115, 83, 198, "Bi", 7754.0, 1913.0};
                bindingData[qMakePair(198, 84)] = {114, 84, 198, "Po", 10190.0, 3075.0};
                bindingData[qMakePair(198, 85)] = {113, 85, 198, "At", 8431.0, 650.0};
                bindingData[qMakePair(198, 86)] = {112, 86, 198, "Rn", 10812.0, 2164.0};
                bindingData[qMakePair(198, 87)] = {111, 87, 198, "Fr", 8750.0, -770.0};

                // A=199
                bindingData[qMakePair(199, 75)] = {124, 75, 199, "Re", 5790.0, 0.0};       // S(p)=*
                bindingData[qMakePair(199, 76)] = {123, 76, 199, "Os", 4720.0, 10630.0};
                bindingData[qMakePair(199, 77)] = {122, 77, 199, "Ir", 6650.0, 7850.0};
                bindingData[qMakePair(199, 78)] = {121, 78, 199, "Pt", 5556.0, 8860.0};
                bindingData[qMakePair(199, 79)] = {120, 79, 199, "Au", 7584.28, 6478.7};
                bindingData[qMakePair(199, 80)] = {119, 80, 199, "Hg", 6663.1, 7254.3};
                bindingData[qMakePair(199, 81)] = {118, 81, 199, "Tl", 8602.0, 4394.0};
                bindingData[qMakePair(199, 82)] = {117, 82, 199, "Pb", 7236.0, 4992.0};
                bindingData[qMakePair(199, 83)] = {116, 83, 199, "Bi", 9499.0, 2019.0};
                bindingData[qMakePair(199, 84)] = {115, 84, 199, "Po", 7806.0, 3130.0};
                bindingData[qMakePair(199, 85)] = {114, 85, 199, "At", 10180.0, 639.0};
                bindingData[qMakePair(199, 86)] = {113, 86, 199, "Rn", 8340.0, 2070.0};
                bindingData[qMakePair(199, 87)] = {112, 87, 199, "Fr", 10870.0, -713.0};

                // A=200
                bindingData[qMakePair(200, 76)] = {124, 76, 200, "Os", 6370.0, 11210.0};
                bindingData[qMakePair(200, 77)] = {123, 77, 200, "Ir", 5280.0, 8420.0};
                bindingData[qMakePair(200, 78)] = {122, 78, 200, "Pt", 7282.0, 9490.0};
                bindingData[qMakePair(200, 79)] = {121, 79, 200, "Au", 6218.0, 7140.0};
                bindingData[qMakePair(200, 80)] = {120, 80, 200, "Hg", 8028.52, 7698.5};
                bindingData[qMakePair(200, 81)] = {119, 81, 200, "Tl", 7059.0, 4790.0};
                bindingData[qMakePair(200, 82)] = {118, 82, 200, "Pb", 9091.0, 5480.0};
                bindingData[qMakePair(200, 83)] = {117, 83, 200, "Bi", 7645.0, 2428.0};
                bindingData[qMakePair(200, 84)] = {116, 84, 200, "Po", 9805.0, 3433.0};
                bindingData[qMakePair(200, 85)] = {115, 85, 200, "At", 8236.0, 1070.0};
                bindingData[qMakePair(200, 86)] = {114, 86, 200, "Rn", 10580.0, 2470.0};
                bindingData[qMakePair(200, 87)] = {113, 87, 200, "Fr", 8710.0, -340.0};
                // A=201
                bindingData[qMakePair(201, 76)] = {125, 76, 201, "Os", 4530.0, 0.0};       // S(p)=*
                bindingData[qMakePair(201, 77)] = {124, 77, 201, "Ir", 6360.0, 8410.0};    // S(p)=8410#
                bindingData[qMakePair(201, 78)] = {123, 78, 201, "Pt", 5210.0, 9420.0};    // S(p)=9420#
                bindingData[qMakePair(201, 79)] = {122, 79, 201, "Au", 7232.0, 7091.0};
                bindingData[qMakePair(201, 80)] = {121, 80, 201, "Hg", 6230.6, 7711.0};
                bindingData[qMakePair(201, 81)] = {120, 81, 201, "Tl", 8205.0, 4967.0};
                bindingData[qMakePair(201, 82)] = {119, 82, 201, "Pb", 7091.0, 5513.0};
                bindingData[qMakePair(201, 83)] = {118, 83, 201, "Bi", 9117.0, 2454.0};
                bindingData[qMakePair(201, 84)] = {117, 84, 201, "Po", 7651.0, 3439.0};
                bindingData[qMakePair(201, 85)] = {116, 85, 201, "At", 9873.0, 137.0};
                bindingData[qMakePair(201, 86)] = {115, 86, 201, "Rn", 8140.0, 2370.0};
                bindingData[qMakePair(201, 87)] = {114, 87, 201, "Fr", 10620.0, -304.0};
                bindingData[qMakePair(201, 88)] = {113, 88, 201, "Ra", 0.0, 1480.0};       // S(n)=*

                // A=202
                bindingData[qMakePair(202, 76)] = {126, 76, 202, "Os", 5920.0, 0.0};       // S(p)=*
                bindingData[qMakePair(202, 77)] = {125, 77, 202, "Ir", 4950.0, 8830.0};    // S(p)=8830#
                bindingData[qMakePair(202, 78)] = {124, 78, 202, "Pt", 7020.0, 10080.0};   // S(p)=10080#
                bindingData[qMakePair(202, 79)] = {123, 79, 202, "Au", 6024.0, 7900.0};
                bindingData[qMakePair(202, 80)] = {122, 80, 202, "Hg", 7754.10, 8234.0};
                bindingData[qMakePair(202, 81)] = {121, 81, 202, "Tl", 6871.0, 5606.6};
                bindingData[qMakePair(202, 82)] = {120, 82, 202, "Pb", 8741.0, 6049.0};
                bindingData[qMakePair(202, 83)] = {119, 83, 202, "Bi", 7396.0, 2759.0};
                bindingData[qMakePair(202, 84)] = {118, 84, 202, "Po", 9492.0, 3814.0};
                bindingData[qMakePair(202, 85)] = {117, 85, 202, "At", 7873.0, 1359.0};
                bindingData[qMakePair(202, 86)] = {116, 86, 202, "Rn", 10270.0, 2774.0};
                bindingData[qMakePair(202, 87)] = {115, 87, 202, "Fr", 8564.0, 120.0};
                bindingData[qMakePair(202, 88)] = {114, 88, 202, "Ra", 10933.0, 1803.0};

                // A=203
                bindingData[qMakePair(203, 76)] = {127, 76, 203, "Os", 2620.0, 0.0};       // S(p)=*
                bindingData[qMakePair(203, 77)] = {126, 77, 203, "Ir", 5990.0, 8890.0};    // S(p)=8890#
                bindingData[qMakePair(203, 78)] = {125, 78, 203, "Pt", 5010.0, 10140.0};   // S(p)=10140#
                bindingData[qMakePair(203, 79)] = {124, 79, 203, "Au", 6862.0, 7740.0};
                bindingData[qMakePair(203, 80)] = {123, 80, 203, "Hg", 5995.3, 8205.0};
                bindingData[qMakePair(203, 81)] = {122, 81, 203, "Tl", 7852.5, 8705.0};
                bindingData[qMakePair(203, 82)] = {121, 82, 203, "Pb", 6917.0, 6095.0};
                bindingData[qMakePair(203, 83)] = {120, 83, 203, "Bi", 8855.0, 2873.0};
                bindingData[qMakePair(203, 84)] = {119, 84, 203, "Po", 7441.0, 3858.0};
                bindingData[qMakePair(203, 85)] = {118, 85, 203, "At", 9643.0, 1510.0};
                bindingData[qMakePair(203, 86)] = {117, 86, 203, "Rn", 7950.0, 2850.0};
                bindingData[qMakePair(203, 87)] = {116, 87, 203, "Fr", 10291.0, 138.0};
                bindingData[qMakePair(203, 88)] = {115, 88, 203, "Ra", 8480.0, 1720.0};

                // A=204
                bindingData[qMakePair(204, 77)] = {127, 77, 204, "Ir", 3070.0, 9340.0};    // S(p)=9340#
                bindingData[qMakePair(204, 78)] = {126, 78, 204, "Pt", 6370.0, 10520.0};   // S(p)=10520#
                bindingData[qMakePair(204, 79)] = {125, 79, 204, "Au", 5580.0, 8310.0};    // S(p)=8310#
                bindingData[qMakePair(204, 80)] = {124, 80, 204, "Hg", 7492.2, 8836.0};
                bindingData[qMakePair(204, 81)] = {123, 81, 204, "Tl", 6656.08, 6365.9};
                bindingData[qMakePair(204, 82)] = {122, 82, 204, "Pb", 8395.0, 6637.5};
                bindingData[qMakePair(204, 83)] = {121, 83, 204, "Bi", 7192.0, 3148.0};
                bindingData[qMakePair(204, 84)] = {120, 84, 204, "Po", 9102.0, 4105.0};
                bindingData[qMakePair(204, 85)] = {119, 85, 204, "At", 7784.0, 1854.0};
                bindingData[qMakePair(204, 86)] = {118, 86, 204, "Rn", 9888.0, 3097.0};
                bindingData[qMakePair(204, 87)] = {117, 87, 204, "Fr", 8340.0, 530.0};
                bindingData[qMakePair(204, 88)] = {116, 88, 204, "Ra", 10680.0, 2109.0};

                // A=205
                bindingData[qMakePair(205, 77)] = {128, 77, 205, "Ir", 4340.0, 0.0};       // S(p)=*
                bindingData[qMakePair(205, 78)] = {127, 78, 205, "Pt", 3120.0, 10570.0};   // S(p)=10570#
                bindingData[qMakePair(205, 79)] = {126, 79, 205, "Au", 6190.0, 8140.0};    // S(p)=8140#
                bindingData[qMakePair(205, 80)] = {125, 80, 205, "Hg", 5669.0, 8930.0};    // S(p)=8930#
                bindingData[qMakePair(205, 81)] = {124, 81, 205, "Tl", 7546.0, 6419.7};
                bindingData[qMakePair(205, 82)] = {123, 82, 205, "Pb", 6731.66, 6713.06};
                bindingData[qMakePair(205, 83)] = {122, 83, 205, "Bi", 8490.0, 3244.0};
                bindingData[qMakePair(205, 84)] = {121, 84, 205, "Po", 7251.0, 4164.0};
                bindingData[qMakePair(205, 85)] = {120, 85, 205, "At", 9168.0, 1920.0};
                bindingData[qMakePair(205, 86)] = {119, 86, 205, "Rn", 7811.0, 3123.0};
                bindingData[qMakePair(205, 87)] = {118, 87, 205, "Fr", 9988.0, 629.0};
                bindingData[qMakePair(205, 88)] = {117, 88, 205, "Ra", 8290.0, 2060.0};
                bindingData[qMakePair(205, 89)] = {116, 89, 205, "Ac", 0.0, -760.0};       // S(n)=*

                // A=206
                bindingData[qMakePair(206, 78)] = {128, 78, 206, "Pt", 4740.0, 10960.0};   // S(p)=10960#
                bindingData[qMakePair(206, 79)] = {127, 79, 206, "Au", 3520.0, 8540.0};    // S(p)=8540#
                bindingData[qMakePair(206, 80)] = {126, 80, 206, "Hg", 6729.0, 9470.0};    // S(p)=9470#
                bindingData[qMakePair(206, 81)] = {125, 81, 206, "Tl", 6503.8, 7255.0};
                bindingData[qMakePair(206, 82)] = {124, 82, 206, "Pb", 8086.66, 7253.7};
                bindingData[qMakePair(206, 83)] = {123, 83, 206, "Bi", 7035.0, 3547.0};
                bindingData[qMakePair(206, 84)] = {122, 84, 206, "Po", 8739.0, 4413.0};
                bindingData[qMakePair(206, 85)] = {121, 85, 206, "At", 7529.0, 2197.0};
                bindingData[qMakePair(206, 86)] = {120, 86, 206, "Rn", 9494.0, 3450.0};
                bindingData[qMakePair(206, 87)] = {119, 87, 206, "Fr", 8004.0, 822.0};
                bindingData[qMakePair(206, 88)] = {118, 88, 206, "Ra", 10340.0, 2414.0};
                bindingData[qMakePair(206, 89)] = {117, 89, 206, "Ac", 8700.0, -350.0};

                // A=207
                bindingData[qMakePair(207, 78)] = {129, 78, 207, "Pt", 2980.0, 0.0};       // S(p)=*
                bindingData[qMakePair(207, 79)] = {128, 79, 207, "Au", 4670.0, 8470.0};    // S(p)=8470#
                bindingData[qMakePair(207, 80)] = {127, 80, 207, "Hg", 3610.0, 9560.0};    // S(p)=9560#
                bindingData[qMakePair(207, 81)] = {126, 81, 207, "Tl", 6852.0, 7378.0};
                bindingData[qMakePair(207, 82)] = {125, 82, 207, "Pb", 6737.78, 7487.7};
                bindingData[qMakePair(207, 83)] = {124, 83, 207, "Bi", 8098.0, 3558.0};
                bindingData[qMakePair(207, 84)] = {123, 84, 207, "Po", 7028.0, 4406.0};
                bindingData[qMakePair(207, 85)] = {122, 85, 207, "At", 8869.0, 2328.0};
                bindingData[qMakePair(207, 86)] = {121, 86, 207, "Rn", 7573.0, 3494.0};
                bindingData[qMakePair(207, 87)] = {120, 87, 207, "Fr", 9670.0, 1000.0};
                bindingData[qMakePair(207, 88)] = {119, 88, 207, "Ra", 8090.0, 2500.0};
                bindingData[qMakePair(207, 89)] = {118, 89, 207, "Ac", 10400.0, -290.0};

                // A=208
                bindingData[qMakePair(208, 78)] = {130, 78, 208, "Pt", 4520.0, 0.0};       // S(p)=*
                bindingData[qMakePair(208, 79)] = {129, 79, 208, "Au", 3360.0, 8850.0};    // S(p)=8850#
                bindingData[qMakePair(208, 80)] = {128, 80, 208, "Hg", 4850.0, 9740.0};    // S(p)=9740#
                bindingData[qMakePair(208, 81)] = {127, 81, 208, "Tl", 3787.0, 7552.0};
                bindingData[qMakePair(208, 82)] = {126, 82, 208, "Pb", 7367.87, 8003.0};
                bindingData[qMakePair(208, 83)] = {125, 83, 208, "Bi", 6886.9, 3707.1};
                bindingData[qMakePair(208, 84)] = {124, 84, 208, "Po", 8395.0, 4703.9};
                bindingData[qMakePair(208, 85)] = {123, 85, 208, "At", 7314.0, 2613.0};
                bindingData[qMakePair(208, 86)] = {122, 86, 208, "Rn", 9092.0, 3717.0};
                bindingData[qMakePair(208, 87)] = {121, 87, 208, "Fr", 7893.0, 1320.0};
                bindingData[qMakePair(208, 88)] = {120, 88, 208, "Ra", 9890.0, 2717.0};
                bindingData[qMakePair(208, 89)] = {119, 89, 208, "Ac", 8460.0, 80.0};
                bindingData[qMakePair(208, 90)] = {118, 90, 208, "Th", 0.0, 1750.0};       // S(n)=*
                // A=209
                   bindingData[qMakePair(209, 79)] = {130, 79, 209, "Au", 4510.0, 8840.0};    // S(p)=8840#
                   bindingData[qMakePair(209, 80)] = {129, 80, 209, "Hg", 3450.0, 9830.0};    // S(p)=9830#
                   bindingData[qMakePair(209, 81)] = {128, 81, 209, "Tl", 4966.0, 7670.0};
                   bindingData[qMakePair(209, 82)] = {127, 82, 209, "Pb", 3937.4, 8153.5};
                   bindingData[qMakePair(209, 83)] = {126, 83, 209, "Bi", 7459.8, 3799.0};
                   bindingData[qMakePair(209, 84)] = {125, 84, 209, "Po", 6967.8, 4784.8};
                   bindingData[qMakePair(209, 85)] = {124, 85, 209, "At", 8484.0, 2702.0};
                   bindingData[qMakePair(209, 86)] = {123, 86, 209, "Rn", 7357.0, 3760.0};
                   bindingData[qMakePair(209, 87)] = {122, 87, 209, "Fr", 9175.0, 1403.0};
                   bindingData[qMakePair(209, 88)] = {121, 88, 209, "Ra", 7941.0, 2765.0};
                   bindingData[qMakePair(209, 89)] = {120, 89, 209, "Ac", 9980.0, 170.0};
                   bindingData[qMakePair(209, 90)] = {119, 90, 209, "Th", 8390.0, 1680.0};    // S(p)=1680#

                   // A=210
                   bindingData[qMakePair(210, 79)] = {131, 79, 210, "Au", 3200.0, 0.0};       // S(p)=*
                   bindingData[qMakePair(210, 80)] = {130, 80, 210, "Hg", 4790.0, 10110.0};   // S(p)=10110#
                   bindingData[qMakePair(210, 81)] = {129, 81, 210, "Tl", 3674.0, 7890.0};    // S(p)=7890#
                   bindingData[qMakePair(210, 82)] = {128, 82, 210, "Pb", 5185.2, 8373.0};
                   bindingData[qMakePair(210, 83)] = {127, 83, 210, "Bi", 4604.63, 4466.3};
                   bindingData[qMakePair(210, 84)] = {126, 84, 210, "Po", 7658.4, 4983.4};
                   bindingData[qMakePair(210, 85)] = {125, 85, 210, "At", 7161.0, 2895.0};
                   bindingData[qMakePair(210, 86)] = {124, 86, 210, "Rn", 8735.0, 4011.0};
                   bindingData[qMakePair(210, 87)] = {123, 87, 210, "Fr", 7635.0, 1681.0};
                   bindingData[qMakePair(210, 88)] = {122, 88, 210, "Ra", 9487.0, 3077.0};
                   bindingData[qMakePair(210, 89)] = {121, 89, 210, "Ac", 8130.0, 360.0};
                   bindingData[qMakePair(210, 90)] = {120, 90, 210, "Th", 10380.0, 2070.0};   // S(p)=2070

                   // A=211
                   bindingData[qMakePair(211, 80)] = {131, 80, 211, "Hg", 3330.0, 10240.0};   // S(p)=10240#
                   bindingData[qMakePair(211, 81)] = {130, 81, 211, "Tl", 4900.0, 8000.0};    // S(p)=8000#
                   bindingData[qMakePair(211, 82)] = {129, 82, 211, "Pb", 3835.8, 8535.0};
                   bindingData[qMakePair(211, 83)] = {128, 83, 211, "Bi", 5138.0, 4420.0};
                   bindingData[qMakePair(211, 84)] = {127, 84, 211, "Po", 4550.8, 4929.6};
                   bindingData[qMakePair(211, 85)] = {126, 85, 211, "At", 7746.0, 2983.1};
                   bindingData[qMakePair(211, 86)] = {125, 86, 211, "Rn", 7222.0, 4072.0};
                   bindingData[qMakePair(211, 87)] = {124, 87, 211, "Fr", 8878.0, 1824.0};
                   bindingData[qMakePair(211, 88)] = {123, 88, 211, "Ra", 7682.0, 3124.0};
                   bindingData[qMakePair(211, 89)] = {122, 89, 211, "Ac", 9660.0, 530.0};
                   bindingData[qMakePair(211, 90)] = {121, 90, 211, "Th", 8220.0, 2170.0};
                   bindingData[qMakePair(211, 91)] = {120, 91, 211, "Pa", 0.0, -730.0};       // S(n)=*

                   // A=212
                   bindingData[qMakePair(212, 80)] = {132, 80, 212, "Hg", 4690.0, 0.0};       // S(p)=*
                   bindingData[qMakePair(212, 81)] = {131, 81, 212, "Tl", 3540.0, 8220.0};    // S(p)=8220#
                   bindingData[qMakePair(212, 82)] = {130, 82, 212, "Pb", 5127.2, 8760.0};
                   bindingData[qMakePair(212, 83)] = {129, 83, 212, "Bi", 4330.0, 4914.0};
                   bindingData[qMakePair(212, 84)] = {128, 84, 212, "Po", 6008.2, 5799.0};
                   bindingData[qMakePair(212, 85)] = {127, 85, 212, "At", 5052.0, 3484.6};
                   bindingData[qMakePair(212, 86)] = {126, 86, 212, "Rn", 7976.0, 4301.0};
                   bindingData[qMakePair(212, 87)] = {125, 87, 212, "Fr", 7447.0, 2050.0};
                   bindingData[qMakePair(212, 88)] = {124, 88, 212, "Ra", 9102.0, 3348.0};
                   bindingData[qMakePair(212, 89)] = {123, 89, 212, "Ac", 8000.0, 840.0};
                   bindingData[qMakePair(212, 90)] = {122, 90, 212, "Th", 9870.0, 2380.0};
                   bindingData[qMakePair(212, 91)] = {121, 91, 212, "Pa", 8560.0, -390.0};    // S(p)=-390
                   // A=213
                   bindingData[qMakePair(213, 80)] = {133, 80, 213, "Hg", 3160.0, 0.0};      // S(p)=*
                   bindingData[qMakePair(213, 81)] = {132, 81, 213, "Tl", 4740.0, 8260.0};   // S(p)=8260#
                   bindingData[qMakePair(213, 82)] = {131, 82, 213, "Pb", 3726.0, 8940.0};   // S(p)=8940#
                   bindingData[qMakePair(213, 83)] = {130, 83, 213, "Bi", 5185.0, 4972.0};
                   bindingData[qMakePair(213, 84)] = {129, 84, 213, "Po", 4355.4, 5825.0};
                   bindingData[qMakePair(213, 85)] = {128, 85, 213, "At", 6023.0, 3499.0};
                   bindingData[qMakePair(213, 86)] = {127, 86, 213, "Rn", 5108.0, 4357.0};
                   bindingData[qMakePair(213, 87)] = {126, 87, 213, "Fr", 8108.0, 2182.0};
                   bindingData[qMakePair(213, 88)] = {125, 88, 213, "Ra", 7527.0, 3427.0};
                   bindingData[qMakePair(213, 89)] = {124, 89, 213, "Ac", 9190.0, 935.0};
                   bindingData[qMakePair(213, 90)] = {123, 90, 213, "Th", 8062.0, 2450.0};
                   bindingData[qMakePair(213, 91)] = {122, 91, 213, "Pa", 10000.0, -260.0};  // S(p)=-260#

                   // A=214
                   bindingData[qMakePair(214, 80)] = {134, 80, 214, "Hg", 4560.0, 0.0};      // S(p)=*
                   bindingData[qMakePair(214, 81)] = {133, 81, 214, "Tl", 3390.0, 8490.0};   // S(p)=8490#
                   bindingData[qMakePair(214, 82)] = {132, 82, 214, "Pb", 5051.0, 9256.0};   // S(p)=9256
                   bindingData[qMakePair(214, 83)] = {131, 83, 214, "Bi", 4040.0, 5286.0};
                   bindingData[qMakePair(214, 84)] = {130, 84, 214, "Po", 5887.8, 5027.0};
                   bindingData[qMakePair(214, 85)] = {129, 85, 214, "At", 4872.0, 4015.0};
                   bindingData[qMakePair(214, 86)] = {128, 86, 214, "Rn", 6695.0, 5029.0};
                   bindingData[qMakePair(214, 87)] = {127, 87, 214, "Fr", 5477.0, 2552.0};
                   bindingData[qMakePair(214, 88)] = {126, 88, 214, "Ra", 8324.0, 3643.0};
                   bindingData[qMakePair(214, 89)] = {125, 89, 214, "Ac", 7782.0, 1191.0};
                   bindingData[qMakePair(214, 90)] = {124, 90, 214, "Th", 9497.0, 2749.0};
                   bindingData[qMakePair(214, 91)] = {123, 91, 214, "Pa", 8250.0, -80.0};    // S(p)=-80

                   // A=215
                   bindingData[qMakePair(215, 80)] = {135, 80, 215, "Hg", 3040.0, 0.0};      // S(p)=*
                   bindingData[qMakePair(215, 81)] = {134, 81, 215, "Tl", 4630.0, 8560.0};   // S(p)=8560#
                   bindingData[qMakePair(215, 82)] = {133, 82, 215, "Pb", 3550.0, 9410.0};   // S(p)=9410#
                   bindingData[qMakePair(215, 83)] = {132, 83, 215, "Bi", 5241.0, 5477.0};
                   bindingData[qMakePair(215, 84)] = {131, 84, 215, "Po", 4143.0, 6630.0};
                   bindingData[qMakePair(215, 85)] = {130, 85, 215, "At", 5921.0, 4075.0};
                   bindingData[qMakePair(215, 86)] = {129, 86, 215, "Rn", 4920.0, 5078.0};
                   bindingData[qMakePair(215, 87)] = {128, 87, 215, "Fr", 6795.0, 2651.0};
                   bindingData[qMakePair(215, 88)] = {127, 88, 215, "Ra", 5630.0, 3797.0};
                   bindingData[qMakePair(215, 89)] = {126, 89, 215, "Ac", 8485.0, 1351.0};
                   bindingData[qMakePair(215, 90)] = {125, 90, 215, "Th", 7845.0, 2811.0};
                   bindingData[qMakePair(215, 91)] = {124, 91, 215, "Pa", 9690.0, 120.0};    // S(p)=120
                   bindingData[qMakePair(215, 92)] = {123, 92, 215, "U", 0.0, 1850.0};       // S(n)=*

                   // A=216
                   bindingData[qMakePair(216, 80)] = {136, 80, 216, "Hg", 4420.0, 0.0};      // S(p)=*
                   bindingData[qMakePair(216, 81)] = {135, 81, 216, "Tl", 3270.0, 8780.0};   // S(p)=8780#
                   bindingData[qMakePair(216, 82)] = {134, 82, 216, "Pb", 4930.0, 9720.0};   // S(p)=9720#
                   bindingData[qMakePair(216, 83)] = {133, 83, 216, "Bi", 3827.0, 5760.0};
                   bindingData[qMakePair(216, 84)] = {132, 84, 216, "Po", 5747.2, 7136.0};
                   bindingData[qMakePair(216, 85)] = {131, 85, 216, "At", 4559.0, 4491.0};
                   bindingData[qMakePair(216, 86)] = {130, 86, 216, "Rn", 6650.0, 5780.0};
                   bindingData[qMakePair(216, 87)] = {129, 87, 216, "Fr", 5418.0, 3149.0};
                   bindingData[qMakePair(216, 88)] = {128, 88, 216, "Ra", 7314.0, 4316.0};
                   bindingData[qMakePair(216, 89)] = {127, 89, 216, "Ac", 5958.0, 1678.0};
                   bindingData[qMakePair(216, 90)] = {126, 90, 216, "Th", 8695.0, 3021.0};
                   bindingData[qMakePair(216, 91)] = {125, 91, 216, "Pa", 8140.0, 410.0};
                   bindingData[qMakePair(216, 92)] = {124, 92, 216, "U", 9930.0, 2090.0};
                   // A=217
                   bindingData[qMakePair(217, 81)] = {136, 81, 217, "Th", 4480.0, 8840.0};   // S(p)=8840#
                   bindingData[qMakePair(217, 82)] = {135, 82, 217, "Pb", 3310.0, 9700.0};   // S(p)=9700#
                   bindingData[qMakePair(217, 83)] = {134, 83, 217, "Bi", 5215.0, 6040.0};   // S(p)=6040#
                   bindingData[qMakePair(217, 84)] = {133, 84, 217, "Po", 3970.0, 7279.0};
                   bindingData[qMakePair(217, 85)] = {132, 85, 217, "At", 5933.0, 4677.0};
                   bindingData[qMakePair(217, 86)] = {131, 86, 217, "Rn", 4666.0, 5887.0};
                   bindingData[qMakePair(217, 87)] = {130, 87, 217, "Fr", 6728.0, 3227.0};
                   bindingData[qMakePair(217, 88)] = {129, 88, 217, "Ra", 5473.0, 4370.0};
                   bindingData[qMakePair(217, 89)] = {128, 89, 217, "Ac", 7512.0, 1876.0};
                   bindingData[qMakePair(217, 90)] = {127, 90, 217, "Th", 6164.0, 3228.0};
                   bindingData[qMakePair(217, 91)] = {126, 91, 217, "Pa", 8800.0, 519.0};
                   bindingData[qMakePair(217, 92)] = {125, 92, 217, "U", 8160.0, 2120.0};    // S(p)=2120#

                   // A=218
                   bindingData[qMakePair(218, 81)] = {137, 81, 218, "Th", 3200.0, 0.0};      // S(p)=*
                   bindingData[qMakePair(218, 82)] = {136, 82, 218, "Pb", 4860.0, 10150.0};  // S(p)=10150#
                   bindingData[qMakePair(218, 83)] = {135, 83, 218, "Bi", 3590.0, 6310.0};   // S(p)=6310#
                   bindingData[qMakePair(218, 84)] = {134, 84, 218, "Po", 5598.0, 7662.0};
                   bindingData[qMakePair(218, 85)] = {133, 85, 218, "At", 4368.0, 5074.0};
                   bindingData[qMakePair(218, 86)] = {132, 86, 218, "Rn", 6512.0, 6466.0};
                   bindingData[qMakePair(218, 87)] = {131, 87, 218, "Fr", 5327.0, 3888.0};
                   bindingData[qMakePair(218, 88)] = {130, 88, 218, "Ra", 7310.0, 4952.0};
                   bindingData[qMakePair(218, 89)] = {129, 89, 218, "Ac", 5930.0, 2340.0};
                   bindingData[qMakePair(218, 90)] = {128, 90, 218, "Th", 7910.0, 3626.0};
                   bindingData[qMakePair(218, 91)] = {127, 91, 218, "Pa", 6456.0, 811.0};
                   bindingData[qMakePair(218, 92)] = {126, 92, 218, "U", 9150.0, 2463.0};

                   // A=219
                   bindingData[qMakePair(219, 82)] = {137, 82, 219, "Pb", 3250.0, 10190.0};  // S(p)=10190#
                   bindingData[qMakePair(219, 83)] = {136, 83, 219, "Bi", 5010.0, 6460.0};   // S(p)=6460#
                   bindingData[qMakePair(219, 84)] = {135, 84, 219, "Po", 3747.0, 7820.0};
                   bindingData[qMakePair(219, 85)] = {134, 85, 219, "At", 5773.0, 5250.0};
                   bindingData[qMakePair(219, 86)] = {133, 86, 219, "Rn", 4459.0, 6588.0};
                   bindingData[qMakePair(219, 87)] = {132, 87, 219, "Fr", 6513.0, 3889.0};
                   bindingData[qMakePair(219, 88)] = {131, 88, 219, "Ra", 5328.0, 4954.0};
                   bindingData[qMakePair(219, 89)] = {130, 89, 219, "Ac", 7350.0, 2370.0};
                   bindingData[qMakePair(219, 90)] = {129, 90, 219, "Th", 5970.0, 3660.0};
                   bindingData[qMakePair(219, 91)] = {128, 91, 219, "Pa", 8210.0, 1120.0};
                   bindingData[qMakePair(219, 92)] = {127, 92, 219, "U", 6680.0, 2690.0};
                   bindingData[qMakePair(219, 93)] = {126, 93, 219, "Np", 0.0, -270.0};      // S(n)=*

                   // A=220
                   bindingData[qMakePair(220, 82)] = {138, 82, 220, "Pb", 4680.0, 0.0};      // S(p)=*
                   bindingData[qMakePair(220, 83)] = {137, 83, 220, "Bi", 3540.0, 6750.0};   // S(p)=6750#
                   bindingData[qMakePair(220, 84)] = {136, 84, 220, "Po", 5489.0, 8310.0};   // S(p)=8310#
                   bindingData[qMakePair(220, 85)] = {135, 85, 220, "At", 4092.0, 3595.0};
                   bindingData[qMakePair(220, 86)] = {134, 86, 220, "Rn", 6288.6, 7073.0};
                   bindingData[qMakePair(220, 87)] = {133, 87, 220, "Fr", 5207.0, 4636.0};
                   bindingData[qMakePair(220, 88)] = {132, 88, 220, "Ra", 7195.0, 5636.0};
                   bindingData[qMakePair(220, 89)] = {131, 89, 220, "Ac", 5900.0, 2940.0};
                   bindingData[qMakePair(220, 90)] = {130, 90, 220, "Th", 7870.0, 4190.0};
                   bindingData[qMakePair(220, 91)] = {129, 91, 220, "Pa", 6390.0, 1540.0};   // S(p)=1540#
                   bindingData[qMakePair(220, 92)] = {128, 92, 220, "U", 8430.0, 2900.0};    // S(p)=2900#
                   bindingData[qMakePair(220, 93)] = {127, 93, 220, "Np", 7220.0, 270.0};    // S(p)=270#

                   //221-225!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                   // A=226 ()
                   bindingData[qMakePair(226, 86)] = {140, 84, 226, "Po", 5050.0, 0.0};      // S(n)=36708, S(p)=*
                   bindingData[qMakePair(226, 87)] = {139, 85, 226, "At", 3850.0, 7210.0};
                   bindingData[qMakePair(226, 86)] = {140, 86, 226, "Rn", 5858.0, 8940.0};      // S(n)=36708, S(p)=*
                   bindingData[qMakePair(226, 87)] = {139, 87, 226, "Fr", 4371.0, 6303.0};    // S(n)=5478, S(p)=7260#
                   bindingData[qMakePair(226, 88)] = {138, 88, 226, "Ra", 6396.0, 7442.0};    // S(n)=4371, S(p)=8559
                   bindingData[qMakePair(226, 89)] = {137, 89, 226, "Ac", 5399.0, 4973.0};    // S(n)=6147, S(p)=6042
                   bindingData[qMakePair(226, 90)] = {136, 90, 226, "Th", 7184.02, 5729.0};   // S(n)=5118.02, S(p)=7311
                   bindingData[qMakePair(226, 91)] = {135, 91, 226, "Pa", 6380.0, 3566.2};     // S(n)=821, S(p)=4777.2
                   bindingData[qMakePair(226, 92)] = {134, 92, 226, "U", 8122.0, 4300.0};     // S(n)=5880, S(p)=5657
                   bindingData[qMakePair(226, 93)] = {133, 93, 226, "Np", 6880.0, 1890.0};    // S(n)=7680#, S(p)=3280



                     // A=227
                     bindingData[qMakePair(227, 84)] = {143, 84, 227, "Po", 3340.0, 0.0};       // S(n)=3340#, S(p)=*
                     bindingData[qMakePair(227, 85)] = {142, 85, 227, "At", 5200.0, 7350.0};    // S(n)=5200#, S(p)=7350#
                     bindingData[qMakePair(227, 86)] = {141, 86, 227, "Rn", 3933.0, 9020.0};    // S(n)=3933, S(p)=9020#
                     bindingData[qMakePair(227, 87)] = {140, 87, 227, "Fr", 5909.0, 6354.0};    // S(n)=5909, S(p)=6354
                     bindingData[qMakePair(227, 88)] = {139, 88, 227, "Ra", 4561.43, 7652.0};   // S(n)=4561.43, S(p)=7652
                     bindingData[qMakePair(227, 89)] = {138, 89, 227, "Ac", 6531.0, 5107.2};    // S(n)=6531, S(p)=5107.2
                     bindingData[qMakePair(227, 90)] = {137, 90, 227, "Th", 5464.0, 5793.0};    // S(n)=5464, S(p)=5793
                     bindingData[qMakePair(227, 91)] = {136, 91, 227, "Pa", 7273.0, 3655.0};    // S(n)=7273, S(p)=3655
                     bindingData[qMakePair(227, 92)] = {135, 92, 227, "U", 6355.0, 4277.0};     // S(n)=6355, S(p)=4277
                     bindingData[qMakePair(227, 93)] = {134, 93, 227, "Np", 8290.0, 2060.0};    // S(n)=8290#, S(p)=2060
                     bindingData[qMakePair(227, 94)] = {133, 94, 227, "Pu", 0.0, 3300.0};       // S(n)=*, S(p)=3300#

                     // A=228
                     bindingData[qMakePair(228, 85)] = {143, 85, 228, "At", 3870.0, 7890.0};    // S(n)=3870#, S(p)=7890#
                     bindingData[qMakePair(228, 86)] = {142, 86, 228, "Rn", 5714.0, 9530.0};    // S(n)=5714, S(p)=9530#
                     bindingData[qMakePair(228, 87)] = {141, 87, 228, "Fr", 4370.0, 6791.0};    // S(n)=4370, S(p)=6791
                     bindingData[qMakePair(228, 88)] = {140, 88, 228, "Ra", 6308.8, 8031.0};    // S(n)=6308.8, S(p)=8031
                     bindingData[qMakePair(228, 89)] = {139, 89, 228, "Ac", 5026.2, 5572.0};    // S(n)=5026.2, S(p)=5572.0
                     bindingData[qMakePair(228, 90)] = {138, 90, 228, "Th", 7105.2, 6367.6};    // S(n)=7105.2, S(p)=6367.6
                     bindingData[qMakePair(228, 91)] = {137, 91, 228, "Pa", 5979.0, 4170.0};    // S(n)=5979, S(p)=4170
                     bindingData[qMakePair(228, 92)] = {136, 92, 228, "U", 7895.0, 4898.0};     // S(n)=7895, S(p)=4898
                     bindingData[qMakePair(228, 93)] = {135, 93, 228, "Np", 7040.0, 2740.0};    // S(n)=7040#, S(p)=2740
                     bindingData[qMakePair(228, 94)] = {134, 94, 228, "Pu", 8750.0, 3760.0};    // S(n)=8750#, S(p)=3760

                     // A=229
                     bindingData[qMakePair(229, 85)] = {144, 85, 229, "At", 4930.0, 0.0};       // S(n)=4930#, S(p)=*
                     bindingData[qMakePair(229, 86)] = {143, 86, 229, "Rn", 3952.0, 9610.0};    // S(n)=3952, S(p)=9610#
                     bindingData[qMakePair(229, 87)] = {142, 87, 229, "Fr", 5787.0, 6864.0};    // S(n)=5787, S(p)=6864
                     bindingData[qMakePair(229, 88)] = {141, 88, 229, "Ra", 4450.0, 8111.0};    // S(n)=4450, S(p)=8111
                     bindingData[qMakePair(229, 89)] = {140, 89, 229, "Ac", 6276.7, 5539.0};    // S(n)=6276.7, S(p)=5539
                     bindingData[qMakePair(229, 90)] = {139, 90, 229, "Th", 5256.7, 6598.1};    // S(n)=5256.7, S(p)=6598.1
                     bindingData[qMakePair(229, 91)] = {138, 91, 229, "Pa", 7098.0, 4163.0};    // S(n)=7098, S(p)=4163
                     bindingData[qMakePair(229, 92)] = {137, 92, 229, "U", 6083.0, 5002.0};     // S(n)=6083, S(p)=5002
                     bindingData[qMakePair(229, 93)] = {136, 93, 229, "Np", 7890.0, 2730.0};    // S(n)=7890#, S(p)=2730
                     bindingData[qMakePair(229, 94)] = {135, 94, 229, "Pu", 6760.0, 3490.0};    // S(n)=6760#, S(p)=3490
                     bindingData[qMakePair(229, 95)] = {134, 95, 229, "Am", 0.0, 1230.0};       // S(n)=*, S(p)=1230

                     // A=230
                     bindingData[qMakePair(230, 86)] = {144, 86, 230, "Rn", 5390.0, 10070.0};  // S(n)=5390#, S(p)=10070#
                     bindingData[qMakePair(230, 87)] = {143, 87, 230, "Fr", 4253.0, 7165.0};    // S(n)=4253, S(p)=7165
                     bindingData[qMakePair(230, 88)] = {142, 88, 230, "Ra", 6117.0, 8411.0};    // S(n)=6117, S(p)=8411
                     bindingData[qMakePair(230, 89)] = {141, 89, 230, "Ac", 4923.0, 6013.0};    // S(n)=4923, S(p)=6013
                     bindingData[qMakePair(230, 90)] = {140, 90, 230, "Th", 6794.3, 7116.0};    // S(n)=6794.3, S(p)=7116
                     bindingData[qMakePair(230, 91)] = {139, 91, 230, "Pa", 5795.0, 4701.0};    // S(n)=5795, S(p)=4701
                     bindingData[qMakePair(230, 92)] = {138, 92, 230, "U", 7667.0, 5571.0};     // S(n)=7667, S(p)=5571
                     bindingData[qMakePair(230, 93)] = {137, 93, 230, "Np", 6610.0, 3260.0};    // S(n)=6610#, S(p)=3260
                     bindingData[qMakePair(230, 94)] = {136, 94, 230, "Pu", 8530.0, 4130.0};    // S(n)=8530#, S(p)=4130
                     bindingData[qMakePair(230, 95)] = {135, 95, 230, "Am", 7290.0, 1750.0};    // S(n)=7290#, S(p)=1750#
                   // A=236
                      bindingData[qMakePair(236, 89)] = {147, 89, 236, "Ac", 4210.0, 7200.0};   // S(n)=4210±40, S(p)=7200#
                      bindingData[qMakePair(236, 90)] = {146, 90, 236, "Th", 5854.0, 8391.0};   // S(n)=5854±19, S(p)=8391±20
                      bindingData[qMakePair(236, 91)] = {145, 91, 236, "Pa", 5026.0, 5973.0};   // S(n)=5026±20, S(p)=5973±19
                      bindingData[qMakePair(236, 92)] = {144, 92, 236, "U", 6545.52, 7133.0};   // S(n)=6545.52±0.26, S(p)=7133±14
                      bindingData[qMakePair(236, 93)] = {143, 93, 236, "Np", 5740.0, 4830.0};   // S(n)=5740±50, S(p)=4830±50
                      bindingData[qMakePair(236, 94)] = {142, 94, 236, "Pu", 7352.0, 5430.5};   // S(n)=7352±21, S(p)=5430.5±1.8
                      bindingData[qMakePair(236, 95)] = {141, 95, 236, "Am", 6660.0, 3430.0};   // S(n)=6660#, S(p)=3430#
                      bindingData[qMakePair(236, 96)] = {140, 96, 236, "Cm", 8250.0, 4060.0};   // S(n)=8250#, S(p)=4060±60
                      bindingData[qMakePair(236, 97)] = {139, 97, 236, "Bk", 7250.0, 1780.0};   // S(n)=7250#, S(p)=1780#

                      // A=237
                      bindingData[qMakePair(237, 89)] = {148, 89, 237, "Ac", 5270.0, 0.0};      // S(n)=5270#, S(p)=*
                      bindingData[qMakePair(237, 90)] = {147, 90, 237, "Th", 4371.0, 8550.0};   // S(n)=4371±21, S(p)=8550±40
                      bindingData[qMakePair(237, 91)] = {146, 91, 237, "Pa", 5878.0, 6071.0};   // S(n)=5878±19, S(p)=6071±19
                      bindingData[qMakePair(237, 92)] = {145, 92, 237, "U", 512.8, 7233.0};     // S(n)=512.8±0.5, S(p)=7233±14
                      bindingData[qMakePair(237, 93)] = {144, 93, 237, "Np", 6580.0, 4861.95};  // S(n)=6580±50, S(p)=4861.95±0.25
                      bindingData[qMakePair(237, 94)] = {143, 94, 237, "Pu", 5881.2, 5580.0};   // S(n)=5881.2±21, S(p)=5580±50
                      bindingData[qMakePair(237, 95)] = {142, 95, 237, "Am", 7540.0, 3620.0};   // S(n)=7540#, S(p)=3620#
                      bindingData[qMakePair(237, 96)] = {141, 96, 237, "Cm", 6680.0, 4080.0};   // S(n)=6680±70, S(p)=4080#
                      bindingData[qMakePair(237, 97)] = {140, 97, 237, "Bk", 8430.0,  1960.0};
                      bindingData[qMakePair(237, 98)] = {139, 98, 237, "Cf", 0.0, 2890.0};      // S(n)=*, S(p)=2890#

                      // A=238
                      bindingData[qMakePair(238, 90)] = {148, 90, 238, "Th", 5500.0, 8780.0};   // S(n)=5500#, S(p)=8780#
                      bindingData[qMakePair(238, 91)] = {147, 91, 238, "Pa", 4705.0, 6350.0};   // S(n)=4705±21, S(p)=6350±22
                      bindingData[qMakePair(238, 92)] = {146, 92, 238, "U", 6153.7, 7750.0};    // S(n)=6153.7±1.3, S(p)=7750±13
                      bindingData[qMakePair(238, 93)] = {145, 93, 238, "Np", 5488.32, 5224.5};  // S(n)=5488.32±0.20, S(p)=5224.5±0.6
                      bindingData[qMakePair(238, 94)] = {144, 94, 238, "Pu", 6999.8, 5997.4};   // S(n)=6999.8±1.3, S(p)=5997.4±0.4
                      bindingData[qMakePair(238, 95)] = {143, 95, 238, "Am", 6220.0, 3960.0};   // S(n)=6220#, S(p)=3960±50
                      bindingData[qMakePair(238, 96)] = {142, 96, 238, "Cm", 7870.0, 4410.0};   // S(n)=7870±70, S(p)=4410#
                      bindingData[qMakePair(238, 97)] = {141, 97, 238, "Bk", 7040.0, 2320.0};   // S(n)=7040#, S(p)=2320#
                      bindingData[qMakePair(238, 98)] = {140, 98, 238, "Cf", 8730.0, 3200.0};   // S(n)=8730#, S(p)=3200#

                      // A=239
                      bindingData[qMakePair(239, 90)] = {149, 90, 239, "Th", 4150.0, 0.0};      // S(n)=4150#, S(p)=*
                      bindingData[qMakePair(239, 91)] = {148, 91, 239, "Pa", 5630.0, 6480.0};   // S(n)=5630#, S(p)=6480#
                      bindingData[qMakePair(239, 92)] = {147, 92, 239, "U", 4806.38, 7610.0};   // S(n)=4806.38±0.17, S(p)=7610±16
                      bindingData[qMakePair(239, 93)] = {146, 93, 239, "Np", 6214.9, 5285.7};   // S(n)=6214.9±1.03, S(p)=5285.7±1.5
                      bindingData[qMakePair(239, 94)] = {145, 94, 239, "Pu", 5646.2, 6155.3};   // S(n)=5646.2±0.33, S(p)=6155.3±0.4
                      bindingData[qMakePair(239, 95)] = {144, 95, 239, "Am", 7100.0, 4061.8};   // S(n)=7100±50, S(p)=4061.8±1.7
                      bindingData[qMakePair(239, 96)] = {143, 96, 239, "Cm", 6370.0, 4560.0};   // S(n)=6370±60, S(p)=4560±70
                      bindingData[qMakePair(239, 97)] = {142, 97, 239, "Bk", 8040.0, 2480.0};   // S(n)=8040#, S(p)=2480#
                      bindingData[qMakePair(239, 98)] = {141, 98, 239, "Cf", 7080.0, 3240.0};   // S(n)=7080#, S(p)=3240#
                      bindingData[qMakePair(239, 99)] = {140, 99, 239, "Es", 0.0, 1010.0};      // S(n)=*, S(p)=1010#

                      // A=240
                      bindingData[qMakePair(240, 91)] = {149, 91, 240, "Pa", 4500.0, 6830.0};   // S(n)=4500#, S(p)=6830#
                      bindingData[qMakePair(240, 92)] = {148, 92, 240, "U", 5928.5, 7910.0};    // S(n)=5928.5±2.9, S(p)=7910#
                      bindingData[qMakePair(240, 93)] = {147, 93, 240, "Np", 5066.0, 5545.0};   // S(n)=5066±17, S(p)=5545±17
                      bindingData[qMakePair(240, 94)] = {146, 94, 240, "Pu", 6534.22, 6474.6};  // S(n)=6534.22±0.23, S(p)=6474.6±1.0
                      bindingData[qMakePair(240, 95)] = {145, 95, 240, "Am", 5952.0, 4367.0};   // S(n)=5952±14, S(p)=4367±14
                      bindingData[qMakePair(240, 96)] = {144, 96, 240, "Cm", 7490.0, 4955.1};   // S(n)=7490±50, S(p)=4955.1±2.3
                      bindingData[qMakePair(240, 97)] = {143, 97, 240, "Bk", 6660.0, 2270.0};   // S(n)=6660#, S(p)=2270#
                      bindingData[qMakePair(240, 98)] = {142, 98, 240, "Cf", 8350.0, 3550.0};   // S(n)=8350#, S(p)=3550#
                      bindingData[qMakePair(240, 99)] = {141, 99, 240, "Es", 7430.0, 1360.0};   // S(n)=7430#, S(p)=1360#

                      // A=241
                      bindingData[qMakePair(241, 91)] = {150, 91, 241, "Pa", 5340.0, 0.0};      // S(n)=5340#, S(p)=*
                      bindingData[qMakePair(241, 92)] = {149, 92, 241, "U", 4590.0, 8000.0};    // S(n)=4590#, S(p)=8000#
                      bindingData[qMakePair(241, 93)] = {148, 93, 241, "Np", 6130.0, 5740.0};      // S(n)=5340#, S(p)=*
                      bindingData[qMakePair(241, 94)] = {147, 94, 241, "Pu", 5241.0, 6650.0};
                      bindingData[qMakePair(241, 95)] = {146, 95, 241, "Am", 6647.0, 4479.0};      // S(n)=5340#, S(p)=*
                      bindingData[qMakePair(241, 96)] = {145, 96, 241, "Cm", 6093.0, 5097.0};
                      bindingData[qMakePair(241, 97)] = {144, 97, 241, "Bk", 7700.0, 2980.0};      // S(n)=5340#, S(p)=*
                      bindingData[qMakePair(241, 98)] = {143, 98, 241, "Cf", 6740.0, 3630.0};
                      bindingData[qMakePair(241, 99)] = {142, 99, 241, "Es", 8410.0, 1420.0};      // S(n)=5340#, S(p)=*
                      bindingData[qMakePair(241, 100)] = {141, 100, 241, "Fm", 0.0, 2360.0};
                      // A=242
                       bindingData[qMakePair(242, 92)] = {150, 92, 242, "U", 5650.0, 8310.0};     // S(n)=5650#, S(p)=8310#
                       bindingData[qMakePair(242, 93)] = {149, 93, 242, "Np", 4010.0, 6070.0};    // S(n)=4010±210, S(p)=6070#
                       bindingData[qMakePair(242, 94)] = {148, 94, 242, "Pu", 6900.0, 6830.0};    // S(n)=6900.0±0.7, S(p)=6830 (ошибка формата)
                       bindingData[qMakePair(242, 95)] = {147, 95, 242, "Am", 5537.4, 4776.07};   // S(n)=5537.4±0.10, S(p)=4776.07±0.19
                       bindingData[qMakePair(242, 96)] = {146, 96, 242, "Cm", 6969.4, 5419.6};    // S(n)=6969.4±1.2, S(p)=5419.6±0.4
                       bindingData[qMakePair(242, 97)] = {145, 97, 242, "Bk", 6370.0, 3260.0};    // S(n)=6370#, S(p)=3260#
                       bindingData[qMakePair(242, 98)] = {144, 98, 242, "Cf", 8010.0, 3930.0};    // S(n)=8010#, S(p)=3930#
                       bindingData[qMakePair(242, 99)] = {143, 99, 242, "Es", 7500.0, 1810.0};    // S(n)=7500#, S(p)=1810#
                       bindingData[qMakePair(242, 100)] = {142, 100, 242, "Fm", 8800.0, 2750.0};  // S(n)=8800#, S(p)=2750#

                       // A=243
                       bindingData[qMakePair(243, 92)] = {151, 92, 243, "U", 4330.0, 0.0};        // S(n)=4330#, S(p)=*
                       bindingData[qMakePair(243, 93)] = {150, 93, 243, "Np", 6610.0, 6030.0};    // S(n)=6610#, S(p)=6030#
                       bindingData[qMakePair(243, 94)] = {149, 94, 243, "Pu", 5033.6, 6950.0};    // S(n)=5033.6±2.4, S(p)=6950±200
                       bindingData[qMakePair(243, 95)] = {148, 95, 243, "Am", 6364.3, 4830.9};    // S(n)=6364.3±1.2, S(p)=4830.9±1.4
                       bindingData[qMakePair(243, 96)] = {147, 96, 243, "Cm", 5693.1, 5575.0};    // S(n)=5693.1±1.0, S(p)=5575.0±1.0
                       bindingData[qMakePair(243, 97)] = {146, 97, 243, "Bk", 7120.0, 3403.0};    // S(n)=7120#, S(p)=3403 (ошибка формата)
                       bindingData[qMakePair(243, 98)] = {145, 98, 243, "Cf", 6470.0, 4030.0};    // S(n)=6470#, S(p)=4030#
                       bindingData[qMakePair(243, 99)] = {144, 99, 243, "Es", 8130.0, 1930.0};    // S(n)=8130#, S(p)=1930#
                       bindingData[qMakePair(243, 100)] = {143, 100, 243, "Fm", 7080.0, 2700.0};  // S(n)=7080#, S(p)=2700#

                       // A=244
                       bindingData[qMakePair(244, 93)] = {151, 93, 244, "Np", 4750.0, 6450.0};    // S(n)=4750#, S(p)=6450#
                       bindingData[qMakePair(244, 94)] = {150, 94, 244, "Pu", 6019.9, 7360.0};    // S(n)=6019.9±2.9, S(p)=7360#
                       bindingData[qMakePair(244, 95)] = {149, 95, 244, "Am", 5367.2, 5164.4};    // S(n)=5367.2±1.6, S(p)=5164.4±2.6
                       bindingData[qMakePair(244, 96)] = {148, 96, 244, "Cm", 6804.1, 6012.1};    // S(n)=6804.1±1.0, S(p)=6012.1±1.2
                       bindingData[qMakePair(244, 97)] = {147, 97, 244, "Bk", 6047.0, 3757.0};    // S(n)=6047±15, S(p)=3757±14
                       bindingData[qMakePair(244, 98)] = {146, 98, 244, "Cf", 7380.0, 4360.0};    // S(n)=7380#, S(p)=4360±5
                       bindingData[qMakePair(244, 99)] = {145, 99, 244, "Es", 6790.0, 2250.0};    // S(n)=6790#, S(p)=2250#
                       bindingData[qMakePair(244, 100)] = {144, 100, 244, "Fm", 8400.0, 3070.0};  // S(n)=8400#, S(p)=3070#

                       // A=245
                       bindingData[qMakePair(245, 93)] = {152, 93, 245, "Np", 5380.0, 0.0};       // S(n)=5380#, S(p)=*
                       bindingData[qMakePair(245, 94)] = {151, 94, 245, "Pu", 6950.0, 7310.0};    // S(n)=6950±1.3, S(p)=7310#
                       bindingData[qMakePair(245, 95)] = {150, 95, 245, "Am", 6050.9, 5194.5};    // S(n)=6050.9±1.9, S(p)=5194.5±2.9
                       bindingData[qMakePair(245, 96)] = {149, 96, 245, "Cm", 5518.6, 6163.6};    // S(n)=5518.6±0.5, S(p)=6163.6±1.1
                       bindingData[qMakePair(245, 97)] = {148, 97, 245, "Bk", 6971.0, 3927.0};    // S(n)=6971±14, S(p)=3927.0±1.4
                       bindingData[qMakePair(245, 98)] = {147, 98, 245, "Cf", 6164.0, 4618.0};    // S(n)=6164±3, S(p)=4618±15
                       bindingData[qMakePair(245, 99)] = {146, 99, 245, "Es", 7730.0, 2400.0};    // S(n)=7730#, S(p)=2400#
                       bindingData[qMakePair(245, 100)] = {145, 100, 245, "Fm", 6850.0, 3130.0};  // S(n)=6850#, S(p)=3130#
                       bindingData[qMakePair(245, 101)] = {144, 101, 245, "Md", 0.0, 980.0};      // S(n)=*, S(p)=980#

                       // A=246
                       bindingData[qMakePair(246, 94)] = {152, 94, 246, "Pu", 5855.0, 7780.0};    // S(n)=5855±20, S(p)=7780#
                       bindingData[qMakePair(246, 95)] = {151, 95, 246, "Am", 5978.0, 5473.0};    // S(n)=5978#, S(p)=5473#
                       bindingData[qMakePair(246, 96)] = {150, 96, 246, "Cm", 6485.9, 6572.5};    // S(n)=6485.9±1.2, S(p)=6572.5±2.0
                       bindingData[qMakePair(246, 97)] = {149, 97, 246, "Bk", 5920.0, 4330.0};    // S(n)=5920±60, S(p)=4330±60
                       bindingData[qMakePair(246, 98)] = {148, 98, 246, "Cf", 7562.2, 5012.5};    // S(n)=7562.2±2.4, S(p)=5012.5±1.8
                       bindingData[qMakePair(246, 99)] = {147, 99, 246, "Es", 6540.0, 2770.0};    // S(n)=6540#, S(p)=2770#
                       bindingData[qMakePair(246, 100)] = {146, 100, 246, "Fm", 8070.0, 3470.0};  // S(n)=8070#, S(p)=3470#
                       bindingData[qMakePair(246, 101)] = {145, 101, 246, "Md", 7230.0, 1360.0};  // S(n)=7230#, S(p)=1360#

                       // A=247
                       bindingData[qMakePair(247, 94)] = {153, 94, 247, "Pu", 4360.0, 0.0};       // S(n)=4360#, S(p)=*
                       bindingData[qMakePair(247, 95)] = {152, 95, 247, "Am", 5910.0, 5530.0};    // S(n)=5910#, S(p)=5530#
                       bindingData[qMakePair(247, 96)] = {151, 96, 247, "Cm", 515.5, 6750.0};     // S(n)=515.5±4, S(p)=6750# (ошибка формата)
                       // A=248
                        bindingData[qMakePair(248, 95)] = {153, 95, 248, "Am", 4660.0, 5830.0};   // S(n)=4660#, S(p)=5830#
                        bindingData[qMakePair(248, 96)] = {152, 96, 248, "Cm", 6212.0, 7050.0};    // S(n)=6212±4, S(p)=7050#
                        bindingData[qMakePair(248, 97)] = {151, 97, 248, "Bk", 5480.0, 4740.0};    // S(n)=5480#, S(p)=4740#
                        bindingData[qMakePair(248, 98)] = {150, 98, 248, "Cf", 6937.0, 5541.0};    // S(n)=6937±16, S(p)=5541±7
                        bindingData[qMakePair(248, 99)] = {149, 99, 248, "Es", 6350.0, 3090.0};    // S(n)=6350#, S(p)=3090#
                        bindingData[qMakePair(248, 100)] = {148, 100, 248, "Fm", 7850.0, 3970.0};  // S(n)=7850#, S(p)=3970±21
                        bindingData[qMakePair(248, 101)] = {147, 101, 248, "Md", 6860.0, 1810.0};  // S(n)=6860#, S(p)=1810#
                        bindingData[qMakePair(248, 102)] = {146, 102, 248, "No", 0.0, 2610.0};     // S(n)=*, S(p)=2610#

                        // A=249
                        bindingData[qMakePair(249, 95)] = {154, 95, 249, "Am", 5530.0, 0.0};       // S(n)=5530#, S(p)=*
                        bindingData[qMakePair(249, 96)] = {153, 96, 249, "Cm", 4713.37, 7100.0};   // S(n)=4713.37±0.25, S(p)=7100#
                        bindingData[qMakePair(249, 97)] = {152, 97, 249, "Bk", 6310.0, 4835.3};    // S(n)=6310#, S(p)=4835.3±2.6
                        bindingData[qMakePair(249, 98)] = {151, 98, 249, "Cf", 5587.0, 5650.0};    // S(n)=5587±5, S(p)=5650#
                        bindingData[qMakePair(249, 99)] = {150, 99, 249, "Es", 7200.0, 3350.0};    // S(n)=7200#, S(p)=3350#
                        bindingData[qMakePair(249, 100)] = {149, 100, 249, "Fm", 6450.0, 4070.0};  // S(n)=6450±10, S(p)=4070#
                        bindingData[qMakePair(249, 101)] = {148, 101, 249, "Md", 7990.0, 1960.0};  // S(n)=7990#, S(p)=1960#
                        bindingData[qMakePair(249, 102)] = {147, 102, 249, "No", 6910.0, 2660.0};  // S(n)=6910#, S(p)=2660#

                        // A=250
                        bindingData[qMakePair(250, 96)] = {154, 96, 250, "Cm", 5832.0, 7400.0};    // S(n)=5832±10, S(p)=7400#
                        bindingData[qMakePair(250, 97)] = {153, 97, 250, "Bk", 4968.0, 5090.0};     // S(n)=4968±4, S(p)=5090±4
                        bindingData[qMakePair(250, 98)] = {152, 98, 250, "Cf", 6623.7, 5965.0};    // S(n)=6623.7±1.3, S(p)=5965.0±1.4
                        bindingData[qMakePair(250, 99)] = {151, 99, 250, "Es", 6020.0, 3790.0};    // S(n)=6020#, S(p)=3790#
                        bindingData[qMakePair(250, 100)] = {150, 100, 250, "Fm", 7518.0, 4390.0};  // S(n)=7518±10, S(p)=4390#
                        bindingData[qMakePair(250, 101)] = {149, 101, 250, "Md", 6670.0, 2180.0};  // S(n)=6670#, S(p)=2180#
                        bindingData[qMakePair(250, 102)] = {148, 102, 250, "No", 8290.0, 2960.0};  // S(n)=8290#, S(p)=2960#

                        // A=251
                        bindingData[qMakePair(251, 96)] = {155, 96, 251, "Cm", 4413.0, 0.0};       // S(n)=4413±25, S(p)=*
                        bindingData[qMakePair(251, 97)] = {154, 97, 251, "Bk", 5793.0, 5051.0};    // S(n)=5793±11, S(p)=5051±15
                        bindingData[qMakePair(251, 98)] = {153, 98, 251, "Cf", 5107.0, 6104.0};    // S(n)=5107±4, S(p)=6104±5
                        bindingData[qMakePair(251, 99)] = {152, 99, 251, "Es", 6780.0, 3947.0};    // S(n)=6780#, S(p)=3947±6
                        bindingData[qMakePair(251, 100)] = {151, 100, 251, "Fm", 6190.0, 4560.0};  // S(n)=6190±17, S(p)=4560#
                        bindingData[qMakePair(251, 101)] = {150, 101, 251, "Md", 7740.0, 2394.0};  // S(n)=7740#, S(p)=2394±20
                        bindingData[qMakePair(251, 102)] = {149, 102, 251, "No", 6790.0, 3070.0};  // S(n)=6790#, S(p)=3070#
                        bindingData[qMakePair(251, 103)] = {148, 103, 251, "Lr", 0.0, 1130.0};     // S(n)=*, S(p)=1130#

                        // A=252
                        bindingData[qMakePair(252, 96)] = {156, 96, 252, "Cm", 5660.0, 0.0};       // S(n)=5660#, S(p)=*
                        bindingData[qMakePair(252, 97)] = {155, 97, 252, "Bk", 4770.0, 5400.0};    // S(n)=4770#, S(p)=5400#
                        bindingData[qMakePair(252, 98)] = {154, 98, 252, "Cf", 6172.0, 6482.0};    // S(n)=6172±4, S(p)=6482±11
                        bindingData[qMakePair(252, 99)] = {153, 99, 252, "Es", 5290.0, 4130.0};    // S(n)=5290±50, S(p)=4130±50
                        bindingData[qMakePair(252, 100)] = {152, 100, 252, "Fm", 7210.0, 4986.0};  // S(n)=7210±16, S(p)=4986±8
                        bindingData[qMakePair(252, 101)] = {151, 101, 252, "Md", 6530.0, 2730.0};  // S(n)=6530#, S(p)=2730#
                        bindingData[qMakePair(252, 102)] = {150, 102, 252, "No", 8050.0, 3384.0};  // S(n)=8050#, S(p)=3384±21
                        bindingData[qMakePair(252, 103)] = {149, 103, 252, "Lr", 7060.0, 1400.0};  // S(n)=7060#, S(p)=1400#

                        // A=253
                        bindingData[qMakePair(253, 97)] = {156, 97, 253, "Bk", 5680.0, 5420.0};    // S(n)=5680#, S(p)=5420#
                        bindingData[qMakePair(253, 98)] = {155, 98, 253, "Cf", 4804.0, 6520.0};    // S(n)=4804±4, S(p)=6520#
                        bindingData[qMakePair(253, 99)] = {154, 99, 253, "Es", 6360.0, 4313.0};    // S(n)=6360±50, S(p)=4313.0±2.6
                        bindingData[qMakePair(253, 100)] = {153, 100, 253, "Fm", 5541.0, 5240.0};  // S(n)=5541±6, S(p)=5240±50
                        bindingData[qMakePair(253, 101)] = {152, 101, 253, "Md", 7410.0, 2930.0};  // S(n)=7410#, S(p)=2930#
                        bindingData[qMakePair(253, 102)] = {151, 102, 253, "No", 6584.0, 3440.0};  // S(n)=6584±12, S(p)=3440#
                        bindingData[qMakePair(253, 103)] = {150, 103, 253, "Lr", 8230.0, 1590.0};  // S(n)=8230#, S(p)=1590#
                        bindingData[qMakePair(253, 104)] = {149, 104, 253, "Rf", 0.0, 2470.0};     // S(n)=*, S(p)=2470#

                        // A=254
                        bindingData[qMakePair(254, 97)] = {157, 97, 254, "Bk", 4610.0, 0.0};       // S(n)=4610#, S(p)=*
                        bindingData[qMakePair(254, 98)] = {156, 98, 254, "Cf", 6031.0, 6880.0};    // S(n)=6031±12, S(p)=6880#
                        bindingData[qMakePair(254, 99)] = {155, 99, 254, "Es", 5091.0, 4600.0};    // S(n)=5091±4, S(p)=4600±6
                        bindingData[qMakePair(254, 100)] = {154, 100, 254, "Fm", 6514.0, 5396.7};  // S(n)=6514±4, S(p)=5396.7±2.3
                        bindingData[qMakePair(254, 101)] = {153, 101, 254, "Md", 5790.0, 3180.0};  // S(n)=5790#, S(p)=3180#
                        bindingData[qMakePair(254, 102)] = {152, 102, 254, "No", 7707.0, 3740.0};  // S(n)=7707±12, S(p)=3740#
                        bindingData[qMakePair(254, 103)] = {151, 103, 254, "Lr", 6780.0, 1780.0};  // S(n)=6780#, S(p)=1780#
                        bindingData[qMakePair(254, 104)] = {150, 104, 254, "Rf", 8430.0, 2670.0};  // S(n)=8430#, S(p)=2670#
                        // A=255
                        bindingData[qMakePair(255, 98)] = {157, 98, 255, "Cf", 4600.0, 6870.0};   // S(n)=4600#, S(p)=6870#
                        bindingData[qMakePair(255, 99)] = {156, 99, 255, "Es", 5973.0, 4541.0};   // S(n)=5973±11, S(p)=4541±16
                        bindingData[qMakePair(255, 100)] = {155, 100, 255, "Fm", 5174.0, 5480.0}; // S(n)=5174±5, S(p)=5480±6
                        bindingData[qMakePair(255, 101)] = {154, 101, 255, "Md", 6680.0, 3349.0}; // S(n)=6680#, S(p)=3349±7
                        bindingData[qMakePair(255, 102)] = {153, 102, 255, "No", 5987.0, 3940.0}; // S(n)=5987±18, S(p)=3940#
                        bindingData[qMakePair(255, 103)] = {152, 103, 255, "Lr", 8000.0, 2065.0}; // S(n)=8000#, S(p)=2065±20
                        bindingData[qMakePair(255, 104)] = {151, 104, 255, "Rf", 6940.0, 2830.0}; // S(n)=6940#, S(p)=2830#
                        bindingData[qMakePair(255, 105)] = {150, 105, 255, "Db", 0.0, 900.0};     // S(n)=*, S(p)=900#

                        // A=256
                        bindingData[qMakePair(256, 98)] = {158, 98, 256, "Cf", 5840.0, 0.0};      // S(n)=5840#, S(p)=*
                        bindingData[qMakePair(256, 99)] = {157, 99, 256, "Es", 4970.0, 4910.0};   // S(n)=4970#, S(p)=4910#
                        bindingData[qMakePair(256, 100)] = {156, 100, 256, "Fm", 6384.0, 5891.0}; // S(n)=6384±7, S(p)=5891±12
                        bindingData[qMakePair(256, 101)] = {155, 101, 256, "Md", 5460.0, 3630.0}; // S(n)=5460#, S(p)=3630#
                        bindingData[qMakePair(256, 102)] = {154, 102, 256, "No", 7056.0, 4310.0}; // S(n)=7056±17, S(p)=4310±10
                        bindingData[qMakePair(256, 103)] = {153, 103, 256, "Lr", 6270.0, 2350.0}; // S(n)=6270±80, S(p)=2350±80
                        bindingData[qMakePair(256, 104)] = {152, 104, 256, "Rf", 8180.0, 3014.0}; // S(n)=8180#, S(p)=3014±25
                        bindingData[qMakePair(256, 105)] = {151, 105, 256, "Db", 7170.0, 1120.0}; // S(n)=7170#, S(p)=1120#

                        // A=257
                        bindingData[qMakePair(257, 99)] = {158, 99, 257, "Es", 5860.0, 4930.0};   // S(n)=5860#, S(p)=4930#
                        bindingData[qMakePair(257, 100)] = {157, 100, 257, "Fm", 4968.0, 5890.0}; // S(n)=4968±6, S(p)=5890#
                        bindingData[qMakePair(257, 101)] = {156, 101, 257, "Md", 6530.0, 3783.0}; // S(n)=6530#, S(p)=3783±6
                        bindingData[qMakePair(257, 102)] = {155, 102, 257, "No", 5646.0, 4500.0}; // S(n)=5646±10, S(p)=4500#
                        bindingData[qMakePair(257, 103)] = {154, 103, 257, "Lr", 7150.0, 2450.0}; // S(n)=7150#, S(p)=2450#
                        bindingData[qMakePair(257, 104)] = {153, 104, 257, "Rf", 6427.0, 3170.0}; // S(n)=6427±21, S(p)=3170±80
                        bindingData[qMakePair(257, 105)] = {152, 105, 257, "Db", 8360.0, 1300.0}; // S(n)=8360#, S(p)=1300#

                        // A=258
                        bindingData[qMakePair(258, 99)] = {159, 99, 258, "Es", 4770.0, 0.0};      // S(n)=4770#, S(p)=*
                        bindingData[qMakePair(258, 100)] = {158, 100, 258, "Fm", 6240.0, 6270.0}; // S(n)=6240#, S(p)=6270#
                        bindingData[qMakePair(258, 101)] = {157, 101, 258, "Md", 5378.0, 4192.0}; // S(n)=5378±4, S(p)=4192±6
                        bindingData[qMakePair(258, 102)] = {156, 102, 258, "No", 6840.0, 4800.0}; // S(n)=6840#, S(p)=4800#
                        bindingData[qMakePair(258, 103)] = {155, 103, 258, "Lr", 5960.0, 2750.0}; // S(n)=5960#, S(p)=2750#
                        bindingData[qMakePair(258, 104)] = {154, 104, 258, "Rf", 7600.0, 3610.0}; // S(n)=7600±30, S(p)=3610#
                        bindingData[qMakePair(258, 105)] = {153, 105, 258, "Db", 6480.0, 1360.0}; // S(n)=6480#, S(p)=1360#
                        bindingData[qMakePair(258, 106)] = {152, 106, 258, "Sg", 5500.0, 2250.0};    // S(n)=, S(p)=2250#

                        // A=259
                        bindingData[qMakePair(259, 100)] = {159, 100, 259, "Fm", 4790.0, 6290.0}; // S(n)=4790#, S(p)=6290#
                        bindingData[qMakePair(259, 101)] = {158, 101, 259, "Md", 6130.0, 4090.0}; // S(n)=6130#, S(p)=4090#
                        bindingData[qMakePair(259, 102)] = {157, 102, 259, "No", 5470.0, 4897.0}; // S(n)=5470#, S(p)=4897±8
                        bindingData[qMakePair(259, 103)] = {156, 103, 259, "Lr", 7000.0, 2920.0}; // S(n)=7000#, S(p)=2920#
                        bindingData[qMakePair(259, 104)] = {155, 104, 259, "Rf", 6050.0, 3710.0}; // S(n)=6050#, S(p)=3710#
                        bindingData[qMakePair(259, 105)] = {154, 105, 259, "Db", 7880.0, 1640.0}; // S(n)=7880#, S(p)=1640±60
                        bindingData[qMakePair(259, 106)] = {153, 106, 259, "Sg", 6800.0, 2570.0}; // S(n)=6800#, S(p)=2570#

                        // A=260
                        bindingData[qMakePair(260, 100)] = {160, 100, 260, "Fm", 6010.0, 0.0};    // S(n)=6010#, S(p)=*
                        bindingData[qMakePair(260, 101)] = {159, 101, 260, "Md", 5140.0, 4440.0}; // S(n)=5140#, S(p)=4440#
                        bindingData[qMakePair(260, 102)] = {158, 102, 260, "No", 6540.0, 5300.0}; // S(n)=6540#, S(p)=5300#
                        bindingData[qMakePair(260, 103)] = {157, 103, 260, "Lr", 5650.0, 3090.0}; // S(n)=5650#, S(p)=3090#
                        bindingData[qMakePair(260, 104)] = {156, 104, 260, "Rf", 7290.0, 3990.0}; // S(n)=7290#, S(p)=3990#
                        bindingData[qMakePair(260, 105)] = {155, 105, 260, "Db", 6390.0, 1980.0}; // S(n)=6390#, S(p)=1980#
                        bindingData[qMakePair(260, 106)] = {154, 106, 260, "Sg", 8040.0, 2730.0}; // S(n)=8040#, S(p)=2730±60
                        bindingData[qMakePair(260, 107)] = {153, 107, 260, "Bh", 0.0, 490.0};     // S(n)=*, S(p)=490#

                        // A=261
                        bindingData[qMakePair(261, 101)] = {160, 101, 261, "Md", 6050.0, 4480.0}; // S(n)=6050#, S(p)=4480#
                        bindingData[qMakePair(261, 102)] = {159, 102, 261, "No", 5230.0, 5390.0}; // S(n)=5230#, S(p)=5390#
                        bindingData[qMakePair(261, 103)] = {158, 103, 261, "Lr", 6790.0, 3340.0}; // S(n)=6790#, S(p)=3340#
                        bindingData[qMakePair(261, 104)] = {157, 104, 261, "Rf", 5900.0, 4250.0}; // S(n)=5900#, S(p)=4250#
                        bindingData[qMakePair(261, 105)] = {156, 105, 261, "Db", 7440.0, 2130.0}; // S(n)=7440#, S(p)=2130#
                        bindingData[qMakePair(261, 106)] = {155, 106, 261, "Sg", 6614.0, 2960.0}; // S(n)=6614±28, S(p)=2960#
                        bindingData[qMakePair(261, 107)] = {154, 107, 261, "Bh", 8260.0, 700.0};  // S(n)=8260#, S(p)=700#
                        // A=262
                        bindingData[qMakePair(262, 101)] = {161, 101, 262, "Md", 5020.0, 0.0};    // S(n)=5020#, S(p)=*
                        bindingData[qMakePair(262, 102)] = {160, 102, 262, "No", 6430.0, 5770.0}; // S(n)=6430#, S(p)=5770#
                        bindingData[qMakePair(262, 103)] = {159, 103, 262, "Lr", 5530.0, 3640.0}; // S(n)=5530#, S(p)=3640#
                        bindingData[qMakePair(262, 104)] = {158, 104, 262, "Rf", 7000.0, 4450.0}; // S(n)=7000#, S(p)=4450#
                        bindingData[qMakePair(262, 105)] = {157, 105, 262, "Db", 6130.0, 2350.0};  // S(n)=6130#, S(p)=2350#
                        bindingData[qMakePair(262, 106)] = {156, 106, 262, "Sg", 7710.0, 3230.0};  // S(n)=7710±40, S(p)=3230#
                        bindingData[qMakePair(262, 107)] = {155, 107, 262, "Bh", 6660.0, 750.0};   // S(n)=6660#, S(p)=750#

                        // A=263
                        bindingData[qMakePair(263, 102)] = {161, 102, 263, "No", 5040.0, 5790.0}; // S(n)=5040#, S(p)=5790#
                        bindingData[qMakePair(263, 103)] = {160, 103, 263, "Lr", 6440.0, 3660.0};  // S(n)=6440#, S(p)=3660#
                        bindingData[qMakePair(263, 104)] = {159, 104, 263, "Rf", 5710.0, 4640.0};  // S(n)=5710#, S(p)=4640#
                        bindingData[qMakePair(263, 105)] = {158, 105, 263, "Db", 7210.0, 2570.0};  // S(n)=7210#, S(p)=2570#
                        bindingData[qMakePair(263, 106)] = {157, 106, 263, "Sg", 6250.0, 3350.0};  // S(n)=6250#, S(p)=3350#
                        bindingData[qMakePair(263, 107)] = {156, 107, 263, "Bh", 8120.0, 1160.0};  // S(n)=8120#, S(p)=1160#
                        bindingData[qMakePair(263, 108)] = {155, 108, 263, "Hs", 0.0, 2150.0};     // S(n)=*, S(p)=2150#

                        // A=264
                        bindingData[qMakePair(264, 102)] = {162, 102, 264, "No", 6190.0, 0.0};     // S(n)=6190#, S(p)=*
                        bindingData[qMakePair(264, 103)] = {161, 103, 264, "Lr", 5420.0, 4040.0};  // S(n)=5420#, S(p)=4040#
                        bindingData[qMakePair(264, 104)] = {160, 104, 264, "Rf", 6750.0, 4940.0};  // S(n)=6750#, S(p)=4940#
                        bindingData[qMakePair(264, 105)] = {159, 105, 264, "Db", 5820.0, 2680.0};  // S(n)=5820#, S(p)=2680#
                        bindingData[qMakePair(264, 106)] = {158, 106, 264, "Sg", 7480.0, 3620.0};  // S(n)=7480#, S(p)=3620#
                        bindingData[qMakePair(264, 107)] = {157, 107, 264, "Bh", 6510.0, 1420.0};  // S(n)=6510#, S(p)=1420#
                        bindingData[qMakePair(264, 108)] = {156, 108, 264, "Hs", 8190.0, 2220.0};  // S(n)=8190#, S(p)=2220#

                        // A=265
                        bindingData[qMakePair(265, 103)] = {162, 103, 265, "Lr", 6220.0, 4070.0};  // S(n)=6220#, S(p)=4070#
                        bindingData[qMakePair(265, 104)] = {161, 104, 265, "Rf", 5460.0, 4980.0};  // S(n)=5460#, S(p)=4980#
                        bindingData[qMakePair(265, 105)] = {160, 105, 265, "Db", 6950.0, 2880.0};  // S(n)=6950#, S(p)=2880#
                        bindingData[qMakePair(265, 106)] = {159, 106, 265, "Sg", 6060.0, 3860.0};  // S(n)=6060#, S(p)=3860#
                        bindingData[qMakePair(265, 107)] = {158, 107, 265, "Bh", 7710.0, 1660.0};  // S(n)=7710#, S(p)=1660#
                        bindingData[qMakePair(265, 108)] = {157, 108, 265, "Hs", 6730.0, 2450.0};  // S(n)=6730±40, S(p)=2450#
                        bindingData[qMakePair(265, 109)] = {156, 109, 265, "Mt", 0.0, 170.0};      // S(n)=*, S(p)=170#

                        // A=266
                        bindingData[qMakePair(266, 103)] = {163, 103, 266, "Lr", 4680.0, 0.0};     // S(n)=4680#, S(p)=*
                        bindingData[qMakePair(266, 104)] = {162, 104, 266, "Rf", 6690.0, 5450.0};  // S(n)=6690#, S(p)=5450#
                        bindingData[qMakePair(266, 105)] = {161, 105, 266, "Db", 5820.0, 3240.0};  // S(n)=5820#, S(p)=3240#
                        bindingData[qMakePair(266, 106)] = {160, 106, 266, "Sg", 7250.0, 4150.0};  // S(n)=7250#, S(p)=4150#
                        bindingData[qMakePair(266, 107)] = {159, 107, 266, "Bh", 6380.0, 1980.0};  // S(n)=6380#, S(p)=1980#
                        bindingData[qMakePair(266, 108)] = {158, 108, 266, "Hs", 7840.0, 2570.0};  // S(n)=7840±50, S(p)=2570#
                        bindingData[qMakePair(266, 109)] = {157, 109, 266, "Mt", 6790.0, 230.0};   // S(n)=6790#, S(p)=230#

                        // A=267
                        bindingData[qMakePair(267, 104)] = {163, 104, 267, "Rf", 4700.0, 5470.0};  // S(n)=4700#, S(p)=5470#
                        bindingData[qMakePair(267, 105)] = {162, 105, 267, "Db", 6730.0, 3290.0};  // S(n)=6730#, S(p)=3290#
                        bindingData[qMakePair(267, 106)] = {161, 106, 267, "Sg", 5880.0, 4220.0};  // S(n)=5880#, S(p)=4220#
                        bindingData[qMakePair(267, 107)] = {160, 107, 267, "Bh", 7410.0, 2140.0};  // S(n)=7410#, S(p)=2140#
                        bindingData[qMakePair(267, 108)] = {159, 108, 267, "Hs", 6560.0, 2740.0};  // S(n)=6560#, S(p)=2740#
                        bindingData[qMakePair(267, 109)] = {158, 109, 267, "Mt", 8240.0, 630.0};   // S(n)=8240#, S(p)=630#
                        bindingData[qMakePair(267, 110)] = {157, 110, 267, "Ds", 0.0, 1370.0};     // S(n)=*, S(p)=1370#

                        // A=268
                        bindingData[qMakePair(268, 104)] = {164, 104, 268, "Rf", 6040.0, 0.0};     // S(n)=6040#, S(p)=*
                        bindingData[qMakePair(268, 105)] = {163, 105, 268, "Db", 5080.0, 3670.0};  // S(n)=5080#, S(p)=3670#
                        bindingData[qMakePair(268, 106)] = {162, 106, 268, "Sg", 7080.0, 4560.0};  // S(n)=7080#, S(p)=4560#
                        bindingData[qMakePair(268, 107)] = {161, 107, 268, "Bh", 6030.0, 2290.0};  // S(n)=6030#, S(p)=2290#
                        bindingData[qMakePair(268, 108)] = {160, 108, 268, "Hs", 7890.0, 3230.0};  // S(n)=7890#, S(p)=3230#
                        bindingData[qMakePair(268, 109)] = {159, 109, 268, "Mt", 6710.0, 790.0};   // S(n)=6710#, S(p)=790#
                        bindingData[qMakePair(268, 110)] = {158, 110, 268, "Ds", 8300.0, 1430.0};  // S(n)=8300#, S(p)=1430#

                        // A=269
                        bindingData[qMakePair(269, 105)] = {164, 105, 269, "Db", 5990.0, 3620.0};  // S(n)=5990#, S(p)=3620#
                        bindingData[qMakePair(269, 106)] = {163, 106, 269, "Sg", 5110.0, 4590.0};  // S(n)=5110#, S(p)=4590#
                        bindingData[qMakePair(269, 107)] = {162, 107, 269, "Bh", 7400.0, 2610.0};  // S(n)=7400#, S(p)=2610#
                        bindingData[qMakePair(269, 108)] = {161, 108, 269, "Hs", 6340.0, 3530.0};  // S(n)=6340#, S(p)=3530#
                        bindingData[qMakePair(269, 109)] = {160, 109, 269, "Mt", 7850.0, 750.0};   // S(n)=7850#, S(p)=750#
                        bindingData[qMakePair(269, 110)] = {159, 110, 269, "Ds", 6890.0, 1610.0};  // S(n)=6890#, S(p)=1610#
                        // A=270
                        bindingData[qMakePair(270, 105)] = {165, 105, 270, "Db", 4910.0, 0.0};    // S(n)=4910#, S(p)=*
                        bindingData[qMakePair(270, 106)] = {164, 106, 270, "Sg", 6340.0, 4950.0}; // S(n)=6340#, S(p)=4950#
                        bindingData[qMakePair(270, 107)] = {163, 107, 270, "Bh", 5320.0, 2830.0}; // S(n)=5320#, S(p)=2830#
                        bindingData[qMakePair(270, 108)] = {162, 108, 270, "Hs", 7520.0, 3650.0}; // S(n)=7520#, S(p)=3650#
                        bindingData[qMakePair(270, 109)] = {161, 109, 270, "Mt", 6730.0, 1140.0}; // S(n)=6730#, S(p)=1140#
                        bindingData[qMakePair(270, 110)] = {160, 110, 270, "Ds", 8230.0, 1980.0}; // S(n)=8230±60, S(p)=1980#

                        // A=271
                        bindingData[qMakePair(271, 106)] = {165, 106, 271, "Sg", 4810.0, 4840.0}; // S(n)=4810#, S(p)=4840#
                        bindingData[qMakePair(271, 107)] = {164, 107, 271, "Bh", 6380.0, 2860.0}; // S(n)=6380#, S(p)=2860#
                        bindingData[qMakePair(271, 108)] = {163, 108, 271, "Hs", 5440.0, 3780.0}; // S(n)=5440#, S(p)=3780#
                        bindingData[qMakePair(271, 109)] = {162, 109, 271, "Mt", 7680.0, 1300.0}; // S(n)=7680#, S(p)=1300#
                        bindingData[qMakePair(271, 110)] = {161, 110, 271, "Ds", 6800.0, 2050.0}; // S(n)=6800#, S(p)=2050#

                        // A=272
                        bindingData[qMakePair(272, 106)] = {166, 106, 272, "Sg", 6250.0, 0.0};    // S(n)=6250#, S(p)=*
                        bindingData[qMakePair(272, 107)] = {165, 107, 272, "Bh", 5200.0, 3260.0}; // S(n)=5200#, S(p)=3260#
                        bindingData[qMakePair(272, 108)] = {164, 108, 272, "Hs", 6810.0, 4200.0}; // S(n)=6810#, S(p)=4200#
                        bindingData[qMakePair(272, 109)] = {163, 109, 272, "Mt", 5590.0, 1450.0}; // S(n)=5590#, S(p)=1450#
                        bindingData[qMakePair(272, 110)] = {162, 110, 272, "Ds", 8000.0, 2380.0}; // S(n)=8000#, S(p)=2380#
                        bindingData[qMakePair(272, 111)] = {161, 111, 272, "Rg", 0.0, 460.0};     // S(n)=*, S(p)=460#

                        // A=273
                        bindingData[qMakePair(273, 106)] = {167, 106, 273, "Sg", 4630.0, 0.0};    // S(n)=4630#, S(p)=*
                        bindingData[qMakePair(273, 107)] = {166, 107, 273, "Bh", 6230.0, 3240.0}; // S(n)=6230#, S(p)=3240#
                        bindingData[qMakePair(273, 108)] = {165, 108, 273, "Hs", 5190.0, 4190.0}; // S(n)=5190#, S(p)=4190#
                        bindingData[qMakePair(273, 109)] = {164, 109, 273, "Mt", 6940.0, 1580.0}; // S(n)=6940#, S(p)=1580#
                        bindingData[qMakePair(273, 110)] = {163, 110, 273, "Ds", 5730.0, 2520.0}; // S(n)=5730#, S(p)=2520#
                        bindingData[qMakePair(273, 111)] = {162, 111, 273, "Rg", 8150.0, 610.0};  // S(n)=8150#, S(p)=610#

                        // A=274
                        bindingData[qMakePair(274, 107)] = {167, 107, 274, "Bh", 5020.0, 3630.0}; // S(n)=5020#, S(p)=3630#
                        bindingData[qMakePair(274, 108)] = {166, 108, 274, "Hs", 6480.0, 4440.0}; // S(n)=6480#, S(p)=4440#
                        bindingData[qMakePair(274, 109)] = {165, 109, 274, "Mt", 5540.0, 1930.0}; // S(n)=5540#, S(p)=1930#
                        bindingData[qMakePair(274, 110)] = {164, 110, 274, "Ds", 7230.0, 2800.0}; // S(n)=7230#, S(p)=2800#
                        bindingData[qMakePair(274, 111)] = {163, 111, 274, "Rg", 6150.0, 1030.0}; // S(n)=6150#, S(p)=1030#

                        // A=275
                        bindingData[qMakePair(275, 107)] = {168, 107, 275, "Bh", 6060.0, 0.0};    // S(n)=6060#, S(p)=*
                        bindingData[qMakePair(275, 108)] = {167, 108, 275, "Hs", 4940.0, 4350.0}; // S(n)=4940#, S(p)=4350#
                        bindingData[qMakePair(275, 109)] = {166, 109, 275, "Mt", 6490.0, 1950.0}; // S(n)=6490#, S(p)=1950#
                        bindingData[qMakePair(275, 110)] = {165, 110, 275, "Ds", 5700.0, 2970.0}; // S(n)=5700#, S(p)=2970#
                        bindingData[qMakePair(275, 111)] = {164, 111, 275, "Rg", 7390.0, 1190.0}; // S(n)=7390#, S(p)=1190#

                        // A=276
                        bindingData[qMakePair(276, 108)] = {168, 108, 276, "Hs", 6410.0, 4690.0}; // S(n)=6410#, S(p)=4690#
                        bindingData[qMakePair(276, 109)] = {167, 109, 276, "Mt", 5590.0, 2590.0}; // S(n)=5590#, S(p)=2590#
                        bindingData[qMakePair(276, 110)] = {166, 110, 276, "Ds", 7100.0, 3580.0}; // S(n)=7100#, S(p)=3580#
                        bindingData[qMakePair(276, 111)] = {165, 111, 276, "Rg", 5880.0, 1370.0}; // S(n)=5880#, S(p)=1370#
                        bindingData[qMakePair(276, 112)] = {164, 112, 276, "Cn", 0.0, 2230.0};    // S(n)=*, S(p)=2230#

                        // A=277
                        bindingData[qMakePair(277, 108)] = {169, 108, 277, "Hs", 4860.0, 0.0};    // S(n)=4860#, S(p)=*
                        bindingData[qMakePair(277, 109)] = {168, 109, 277, "Mt", 6420.0, 2610.0}; // S(n)=6420#, S(p)=2610#
                        bindingData[qMakePair(277, 110)] = {167, 110, 277, "Ds", 5470.0, 3460.0}; // S(n)=5470#, S(p)=3460#
                        bindingData[qMakePair(277, 111)] = {166, 111, 277, "Rg", 7220.0, 1490.0}; // S(n)=7220#, S(p)=1490#
                        bindingData[qMakePair(277, 112)] = {165, 112, 277, "Cn", 6020.0, 2370.0}; // S(n)=6020#, S(p)=2370#

                        // A=278
                        bindingData[qMakePair(278, 109)] = {169, 109, 278, "Mt", 5300.0, 3050.0}; // S(n)=5300#, S(p)=3050#
                        bindingData[qMakePair(278, 110)] = {168, 110, 278, "Ds", 6830.0, 3880.0}; // S(n)=6830#, S(p)=3880#
                        bindingData[qMakePair(278, 111)] = {167, 111, 278, "Rg", 5890.0, 1910.0}; // S(n)=5890#, S(p)=1910#
                        bindingData[qMakePair(278, 112)] = {166, 112, 278, "Cn", 7540.0, 2690.0}; // S(n)=7540#, S(p)=2690#
                        bindingData[qMakePair(278, 113)] = {165, 113, 278, "Ed", 0.0, 800.0};     // S(n)=*, S(p)=800#
                        // A=279
                        bindingData[qMakePair(279, 109)] = {170, 109, 279, "Mt", 6430.0, 0.0};    // S(n)=6430#, S(p)=*
                        bindingData[qMakePair(279, 110)] = {169, 110, 279, "Ds", 5220.0, 3760.0}; // S(n)=5220#, S(p)=3760#
                        bindingData[qMakePair(279, 111)] = {168, 111, 279, "Rg", 6930.0, 2000.0}; // S(n)=6930#, S(p)=2000#
                        bindingData[qMakePair(279, 112)] = {167, 112, 279, "Cn", 5850.0, 3290.0}; // S(n)=5850#, S(p)=3290#
                        bindingData[qMakePair(279, 113)] = {166, 113, 279, "Ed", 7730.0, 960.0};  // S(n)=7730#, S(p)=960#

                        // A=280
                        bindingData[qMakePair(280, 110)] = {170, 110, 280, "Ds", 6940.0, 4270.0}; // S(n)=6940#, S(p)=4270#
                        bindingData[qMakePair(280, 111)] = {169, 111, 280, "Rg", 5820.0, 2500.0}; // S(n)=5820#, S(p)=2500#
                        bindingData[qMakePair(280, 112)] = {168, 112, 280, "Cn", 7510.0, 3170.0}; // S(n)=7510#, S(p)=3170#
                        bindingData[qMakePair(280, 113)] = {167, 113, 280, "Ed", 6230.0, 1340.0}; // S(n)=6230#, S(p)=1340#

                        // A=281
                        bindingData[qMakePair(281, 110)] = {171, 110, 281, "Ds", 5090.0, 0.0};    // S(n)=5090#, S(p)=*
                        bindingData[qMakePair(281, 111)] = {170, 111, 281, "Rg", 6590.0, 2590.0}; // S(n)=6590#, S(p)=2590#
                        bindingData[qMakePair(281, 112)] = {169, 112, 281, "Cn", 5650.0, 3000.0}; // S(n)=5650#, S(p)=3000#
                        bindingData[qMakePair(281, 113)] = {168, 113, 281, "Ed", 7560.0, 1390.0}; // S(n)=7560#, S(p)=1390#

                        // A=282
                        bindingData[qMakePair(282, 111)] = {171, 111, 282, "Rg", 5500.0, 3000.0}; // S(n)=5500#, S(p)=3000#
                        bindingData[qMakePair(282, 112)] = {170, 112, 282, "Cn", 7370.0, 3440.0}; // S(n)=7370#, S(p)=3440#
                        bindingData[qMakePair(282, 113)] = {169, 113, 282, "Ed", 6030.0, 1770.0}; // S(n)=6030#, S(p)=1770#

                        // A=283
                        bindingData[qMakePair(283, 111)] = {172, 111, 283, "Rg", 6740.0, 0.0};    // S(n)=6740#, S(p)=*
                        bindingData[qMakePair(283, 112)] = {171, 112, 283, "Cn", 5490.0, 3420.0}; // S(n)=5490#, S(p)=3420#
                        bindingData[qMakePair(283, 113)] = {170, 113, 283, "Ed", 7240.0, 1650.0}; // S(n)=7240#, S(p)=1650#

                        // A=284
                        bindingData[qMakePair(284, 112)] = {172, 112, 284, "Cn", 7250.0, 3920.0}; // S(n)=7250#, S(p)=3920#
                        bindingData[qMakePair(284, 113)] = {171, 113, 284, "Ed", 6060.0, 2210.0}; // S(n)=6060#, S(p)=2210#

                        // A=285
                        bindingData[qMakePair(285, 112)] = {173, 112, 285, "Cn", 5310.0, 0.0};    // S(n)=5310#, S(p)=*
                        bindingData[qMakePair(285, 113)] = {172, 113, 285, "Ed", 7140.0, 2100.0}; // S(n)=7140#, S(p)=2100#
                        bindingData[qMakePair(285, 114)] = {171, 114, 285, "Fl", 0.0, 2710.0};    // S(n)=*, S(p)=2710#

                        // A=286
                        bindingData[qMakePair(286, 113)] = {173, 113, 286, "Ed", 5760.0, 2550.0}; // S(n)=5760#, S(p)=2550#
                        bindingData[qMakePair(286, 114)] = {172, 114, 286, "Fl", 7520.0, 3090.0}; // S(n)=7520#, S(p)=3090#

                        // A=287
                        bindingData[qMakePair(287, 113)] = {174, 113, 287, "Ed", 6970.0, 0.0};    // S(n)=6970#, S(p)=*
                        bindingData[qMakePair(287, 114)] = {173, 114, 287, "Fl", 5700.0, 3030.0}; // S(n)=5700#, S(p)=3030#
                        bindingData[qMakePair(287, 115)] = {172, 115, 287, "Er", 0.0, 1260.0};    // S(n)=*, S(p)=1260#

                        // A=288
                        bindingData[qMakePair(288, 114)] = {174, 114, 288, "Fl", 7340.0, 3390.0}; // S(n)=7340#, S(p)=3390#
                        bindingData[qMakePair(288, 115)] = {173, 115, 288, "Er", 6180.0, 1740.0}; // S(n)=6180#, S(p)=1740#

                        // A=289
                        bindingData[qMakePair(289, 114)] = {175, 114, 289, "Fl", 5420.0, 0.0};    // S(n)=5420#, S(p)=*
                        bindingData[qMakePair(289, 115)] = {174, 115, 289, "Er", 7250.0, 1650.0}; // S(n)=7250#, S(p)=1650#
                        bindingData[qMakePair(289, 116)] = {173, 116, 289, "Lv", 0.0, 2240.0};    // S(n)=*, S(p)=2240#

                        // A=290
                        bindingData[qMakePair(290, 115)] = {175, 115, 290, "Er", 5880.0, 2110.0}; // S(n)=5880#, S(p)=2110#
                        bindingData[qMakePair(290, 116)] = {174, 116, 290, "Lv", 7630.0, 2620.0}; // S(n)=7630#, S(p)=2620#

                        // A=291
                        bindingData[qMakePair(291, 115)] = {176, 115, 291, "Er", 7050.0, 0.0};    // S(n)=7050#, S(p)=*
                        bindingData[qMakePair(291, 116)] = {175, 116, 291, "Lv", 5800.0, 2540.0}; // S(n)=5800#, S(p)=2540#
                        bindingData[qMakePair(291, 117)] = {174, 117, 291, "Eh", 0.0, 870.0};     // S(n)=*, S(p)=870#

                        // A=292
                        bindingData[qMakePair(292, 116)] = {176, 116, 292, "Lv", 7450.0, 2940.0}; // S(n)=7450#, S(p)=2940#
                        bindingData[qMakePair(292, 117)] = {175, 117, 292, "Eh", 6280.0, 1340.0}; // S(n)=6280#, S(p)=1340#

                        // A=293
                        bindingData[qMakePair(293, 116)] = {177, 116, 293, "Lv", 5510.0, 0.0};    // S(n)=5510#, S(p)=*
                        bindingData[qMakePair(293, 117)] = {176, 117, 293, "Eh", 7350.0, 1240.0}; // S(n)=7350#, S(p)=1240#
                        bindingData[qMakePair(293, 118)] = {175, 118, 293, "Ei", 0.0, 1610.0};    // S(n)=*, S(p)=1610#

                        // A=294
                        bindingData[qMakePair(294, 117)] = {177, 117, 294, "Eh", 6000.0, 1720.0}; // S(n)=6000#, S(p)=1720#
                        bindingData[qMakePair(294, 118)] = {176, 118, 294, "Ei", 7740.0, 1990.0}; // S(n)=7740#, S(p)=1990#

                        // A=295
                        bindingData[qMakePair(295, 118)] = {177, 118, 295, "Ei", 5910.0, 1910.0}; // S(n)=5910#, S(p)=1910#


}

