#include "standard_curves.h"

#include <iterator>
#include <stdexcept>
#include <string_view>

namespace dstu4145
{
    namespace
    {
        struct curve_data
        {
            int m;
            int k1, k2, k3; // k2 == 0 for a trinomial basis
            int a;
            std::string_view b;
            std::string_view n;
            std::string_view px;
            std::string_view py;
        };

        constexpr curve_data curves[] = {
            // uacurve0
            {
                163, 7, 6, 3, 1,
                "05FF6108462A2DC8210AB403925E638A19C1455D21",
                "0400000000000000000002BEC12BE2262D39BCF14D",
                "02E2F85F5DD74CE983A5C4237229DAF8A3F35823BE",
                "03826F008A8C51D7B95284D9D03FF0E00CE2CD723A",
            },
            // uacurve1
            {
                167, 6, 0, 0, 1,
                "6EE3CEEB230811759F20518A0930F1A4315A827DAC",
                "3FFFFFFFFFFFFFFFFFFFFFB12EBCC7D7F29FF7701F",
                "7A1F6653786A68192803910A3D30B2A2018B21CD54",
                "5F49EB26781C0EC6B8909156D98ED435E45FD59918",
            },
            // uacurve2
            {
                173, 10, 2, 1, 0,
                "108576C80499DB2FC16EDDF6853BBB278F6B6FB437D9",
                "0800000000000000000000189B4E67606E3825BB2831",
                "04D41A619BCC6EADF0448FA22FAD567A9181D37389CA",
                "10B51CC12849B234C75E6DD2028BF7FF5C1CE0D991A1",
            },
            // uacurve3
            {
                179, 4, 2, 1, 1,
                "04A6E0856526436F2F88DD07A341E32D04184572BEB710",
                "03FFFFFFFFFFFFFFFFFFFFFFB981960435FE5AB64236EF",
                "06BA06FE51464B2BD26DC57F48819BA9954667022C7D03",
                "025FBC363582DCEC065080CA8287AAFF09788A66DC3A9E",
            },
            // uacurve4
            {
                191, 9, 0, 0, 1,
                "7BC86E2102902EC4D5890E8B6B4981FF27E0482750FEFC03",
                "40000000000000000000000069A779CAC1DABC6788F7474F",
                "714114B762F2FF4A7912A6D2AC58B9B5C2FCFE76DAEB7129",
                "29C41E568B77C617EFE5902F11DB96FA9613CD8D03DB08DA",
            },
            // uacurve5
            {
                233, 9, 4, 1, 1,
                "006973B15095675534C7CF7E64A21BD54EF5DD3B8A0326AA936ECE454D2C",
                "01000000000000000000000000000013E974E72F8A6922031D2603CFE0D7",
                "003FCDA526B6CDF83BA1118DF35B3C31761D3545F32728D003EEB25EFE96",
                "009CA8B57A934C54DEEDA9E54A7BBAD95E3B2E91C54D32BE0B9DF96D8D35",
            },
            // uacurve6
            {
                257, 12, 0, 0, 0,
                "01CEF494720115657E18F938D7A7942394FF9425C1458C57861F9EEA6ADBE3BE10",
                "00800000000000000000000000000000006759213AF182E987D3E17714907D470D",
                "002A29EF207D0E9B6C55CD260B306C7E007AC491CA1B10C62334A9E8DCD8D20FB7",
                "010686D41FF744D4449FCCF6D8EEA03102E6812C93A9D60B978B702CF156D814EF",
            },
            // uacurve7
            {
                307, 8, 4, 2, 1,
                "0393C7F7D53666B5054B5E6C6D3DE94F4296C0C599E2E2E241050DF18B6090BDC90186904968BB",
                "03FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC079C2F3825DA70D390FBBA588D4604022B7B7",
                "0216EE8B189D291A0224984C1E92F1D16BF75CCD825A087A239B276D3167743C52C02D6E7232AA",
                "05D9306BACD22B7FAEB09D2E049C6E2866C5D1677762A8F2F2DC9A11C7F7BE8340AB2237C7F2A0",
            },
            // uacurve8
            {
                367, 21, 0, 0, 1,
                "43FC8AD242B0B7A6F3D1627AD5654447556B47BF6AA4A64B0C2AFE42CADAB8F93D92394C79A79755437B56995136",
                "40000000000000000000000000000000000000000000009C300B75A3FA824F22428FD28CE8812245EF44049B2D49",
                "324A6EDDD512F08C49A99AE0D3F961197A76413E7BE81A400CA681E09639B5FE12E59A109F78BF4A373541B3B9A1",
                "01AB597A5B4477F59E39539007C7F977D1A567B92B043A49C6B61984C3FE3481AAF454CD41BA1F051626442B3C10",
            },
            // uacurve9
            {
                431, 5, 3, 1, 1,
                "03CE10490F6A708FC26DFE8C3D27C4F94E690134D5BFF988D8D28AAEAEDE975936C66BAC536B18AE2DC312CA493117DAA469C640CAF3",
                "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBA3175458009A8C0A724F02F81AA8A1FCBAF80D90C7A95110504CF",
                "1A62BA79D98133A16BBAE7ED9A8E03C32E0824D57AEF72F88986874E5AAE49C27BED49A2A95058068426C2171E99FD3B43C5947C857D",
                "70B5E1E14031C1F70BBEFE96BDDE66F451754B4CA5F48DA241F331AA396B8D1839A855C1769B1EA14BA53308B5E2723724E090E02DB9",
            },
        };

        auto field(const curve_data& c) -> gf2m
        {
            return c.k2 ? gf2m{c.m, c.k1, c.k2, c.k3} : gf2m{c.m, c.k1};
        }
    }

    auto standard_curve(curve_id id) -> domain_params
    {
        const auto ix = static_cast<size_t>(id);
        if (ix >= std::size(curves))
            throw std::runtime_error("unknown curve");

        const auto& c = curves[ix];
        auto curve = ecurve{field(c), c.a, integer{c.b}};
        auto p = ecurve_point{curve, integer{c.px}, integer{c.py}};

        return domain_params{curve, integer{c.n}, p};
    }
}
