// 勾配グラフテスト.cpp : 勾配グラフをテストします
//
// Copyright © 2020 Watanabe, Yuki
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301  USA

#include "pch.h"
#include "CppUnitTest.h"
#include "bve-autopilot/勾配グラフ.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace autopilot;

namespace bveautopilottest
{
    TEST_CLASS(勾配加速度グラフテスト)
    {
    public:

        TEST_METHOD(空のグラフの勾配加速度)
        {
            勾配加速度グラフ g;
            Assert::IsTrue(g.empty());
            Assert::AreEqual(0.0, g.勾配加速度(0.0_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(1.0_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(5.0_m).value, 0.0);
        }

        TEST_METHOD(勾配一つのグラフの勾配加速度)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.02);
            Assert::IsFalse(g.empty());
            Assert::AreEqual(0.0, g.勾配加速度(0.0_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(9.9_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(10.0_m).value, 0.0);
            Assert::AreEqual(-0.147, g.勾配加速度(20.0_m).value, 0.001);
            Assert::AreEqual(-0.147, g.勾配加速度(30.0_m).value, 0.001);
            Assert::AreEqual(-0.074, g.勾配加速度(15.0_m).value, 0.001);
            Assert::AreEqual(-0.037, g.勾配加速度(12.5_m).value, 0.001);
        }

        TEST_METHOD(離れた勾配二つのグラフの勾配加速度_昇順)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            g.勾配変化追加({40.0_m, 45.0_m}, -0.02);
            Assert::AreEqual(0.0, g.勾配加速度(9.9_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(10.0_m).value, 0.0);
            Assert::AreEqual(-0.0184, g.勾配加速度(12.5_m).value, 0.0001);
            Assert::AreEqual(-0.074, g.勾配加速度(20.0_m).value, 0.001);
            Assert::AreEqual(-0.074, g.勾配加速度(20.1_m).value, 0.001);
            Assert::AreEqual(-0.074, g.勾配加速度(40.0_m).value, 0.001);
            Assert::AreEqual(0.074, g.勾配加速度(45.0_m).value, 0.001);
            Assert::AreEqual(0.0, g.勾配加速度(42.5_m).value, 0.001);
        }

        TEST_METHOD(離れた勾配二つのグラフの勾配加速度_降順)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({40.0_m, 45.0_m}, -0.02);
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            Assert::AreEqual(0.0, g.勾配加速度(9.9_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(10.0_m).value, 0.0);
            Assert::AreEqual(-0.0184, g.勾配加速度(12.5_m).value, 0.0001);
            Assert::AreEqual(-0.074, g.勾配加速度(20.0_m).value, 0.001);
            Assert::AreEqual(-0.074, g.勾配加速度(20.1_m).value, 0.001);
            Assert::AreEqual(-0.074, g.勾配加速度(40.0_m).value, 0.001);
            Assert::AreEqual(0.074, g.勾配加速度(45.0_m).value, 0.001);
            Assert::AreEqual(0.0, g.勾配加速度(42.5_m).value, 0.001);
        }

        TEST_METHOD(重なった勾配二つのグラフの勾配加速度_昇順)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({100.0_m, 120.0_m}, 0.005);
            g.勾配変化追加({110.0_m, 130.0_m}, -0.015);
            Assert::AreEqual(0.0, g.勾配加速度(99.9_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(100.0_m).value, 0.0);
            Assert::AreEqual(-0.009, g.勾配加速度(105.0_m).value, 0.001);
            Assert::AreEqual(-0.018, g.勾配加速度(110.0_m).value, 0.001);
            Assert::AreEqual(0.0, g.勾配加速度(115.0_m).value, 0.001);
            Assert::AreEqual(0.018, g.勾配加速度(120.0_m).value, 0.001);
            Assert::AreEqual(0.046, g.勾配加速度(125.0_m).value, 0.001);
            Assert::AreEqual(0.074, g.勾配加速度(130.0_m).value, 0.001);
            Assert::AreEqual(0.074, g.勾配加速度(130.1_m).value, 0.001);
        }

        TEST_METHOD(重なった勾配二つのグラフの勾配加速度_降順)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({110.0_m, 130.0_m}, -0.015);
            g.勾配変化追加({100.0_m, 120.0_m}, 0.005);
            Assert::AreEqual(0.0, g.勾配加速度(99.9_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(100.0_m).value, 0.0);
            Assert::AreEqual(-0.009, g.勾配加速度(105.0_m).value, 0.001);
            Assert::AreEqual(-0.018, g.勾配加速度(110.0_m).value, 0.001);
            Assert::AreEqual(0.0, g.勾配加速度(115.0_m).value, 0.001);
            Assert::AreEqual(0.018, g.勾配加速度(120.0_m).value, 0.001);
            Assert::AreEqual(0.046, g.勾配加速度(125.0_m).value, 0.001);
            Assert::AreEqual(0.074, g.勾配加速度(130.0_m).value, 0.001);
            Assert::AreEqual(0.074, g.勾配加速度(130.1_m).value, 0.001);
        }

        TEST_METHOD(一致する勾配二つのグラフの勾配加速度)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.04);
            g.勾配変化追加({10.0_m, 20.0_m}, -0.02);
            Assert::AreEqual(0.0, g.勾配加速度(9.9_m).value, 0.0);
            Assert::AreEqual(0.0, g.勾配加速度(10.0_m).value, 0.0);
            Assert::AreEqual(-0.147, g.勾配加速度(20.0_m).value, 0.001);
            Assert::AreEqual(-0.147, g.勾配加速度(20.1_m).value, 0.001);
            Assert::AreEqual(-0.074, g.勾配加速度(15.0_m).value, 0.001);
            Assert::AreEqual(-0.037, g.勾配加速度(12.5_m).value, 0.001);
        }

        TEST_METHOD(空のグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            Assert::AreEqual(0.0, g.比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(0.0, g.比エネルギー差({0.0_m, 1.0_m}).value, 0.0);
            Assert::AreEqual(0.0, g.比エネルギー差({5.0_m, 7.7_m}).value, 0.0);
        }

        TEST_METHOD(負の勾配一つのグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            Assert::AreEqual(
                0.0, g.比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                -0.07, g.比エネルギー差({12.5_m, 15.0_m}).value, 0.01);
            Assert::AreEqual(
                -0.02, g.比エネルギー差({5.0_m, 12.5_m}).value, 0.01);
            Assert::AreEqual(
                -0.35, g.比エネルギー差({15.0_m, 21.0_m}).value, 0.01);
        }

        TEST_METHOD(正の勾配一つのグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, -0.01);
            Assert::AreEqual(
                0.0, g.比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.比エネルギー差({0.0_m, 9.9_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.比エネルギー差({0.0_m, 10.0_m}).value, 0.0);
            Assert::AreEqual(
                0.37, g.比エネルギー差({0.0_m, 20.0_m}).value, 0.01);
            Assert::AreEqual(
                1.10, g.比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
        }

        TEST_METHOD(負の部分的勾配のグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, 0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, -0.01);
            Assert::AreEqual(
                -1.47, g.比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
            Assert::AreEqual(
                -0.74, g.比エネルギー差({0.0_m, 15.0_m}).value, 0.01);
            Assert::AreEqual(
                -1.47, g.比エネルギー差({0.0_m, 40.0_m}).value, 0.01);
        }

        TEST_METHOD(正の部分的勾配のグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, -0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, 0.01);
            Assert::AreEqual(
                1.47, g.比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
            Assert::AreEqual(
                0.74, g.比エネルギー差({0.0_m, 15.0_m}).value, 0.01);
            Assert::AreEqual(
                1.47, g.比エネルギー差({0.0_m, 40.0_m}).value, 0.01);
        }

        TEST_METHOD(負から正になる勾配のグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, 0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, -0.04);
            Assert::AreEqual(
                0.0, g.比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                -0.37, g.比エネルギー差({0.0_m, 10.0_m}).value, 0.01);
            Assert::AreEqual(
                -1.10, g.比エネルギー差({0.0_m, 20.0_m}).value, 0.01);
            Assert::AreEqual(
                -1.20, g.比エネルギー差({0.0_m, 22.5_m}).value, 0.01);
            Assert::AreEqual(
                -1.10, g.比エネルギー差({0.0_m, 25.0_m}).value, 0.01);
            Assert::AreEqual(
                -0.83, g.比エネルギー差({0.0_m, 27.5_m}).value, 0.01);
            Assert::AreEqual(
                -0.37, g.比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
            Assert::AreEqual(
                0.74, g.比エネルギー差({0.0_m, 35.0_m}).value, 0.01);
        }

        TEST_METHOD(正から負になる勾配のグラフの比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, -0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, 0.05);
            Assert::AreEqual(
                1.10, g.比エネルギー差({0.0_m, 20.0_m}).value, 0.01);
            Assert::AreEqual(
                1.16, g.比エネルギー差({0.0_m, 21.0_m}).value, 0.01);
            Assert::AreEqual(
                1.18, g.比エネルギー差({0.0_m, 22.0_m}).value, 0.01);
            Assert::AreEqual(
                1.16, g.比エネルギー差({0.0_m, 23.0_m}).value, 0.01);
            Assert::AreEqual(
                0.0, g.比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
            Assert::AreEqual(
                -1.47, g.比エネルギー差({0.0_m, 35.0_m}).value, 0.01);
        }

        TEST_METHOD(空のグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 1.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({5.0_m, 7.7_m}).value, 0.0);
        }

        TEST_METHOD(負の勾配一つのグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({12.5_m, 15.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({5.0_m, 12.5_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({15.0_m, 21.0_m}).value, 0.0);
        }

        TEST_METHOD(正の勾配一つのグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, -0.01);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 9.9_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 10.0_m}).value, 0.0);
            Assert::AreEqual(
                0.37, g.下り勾配比エネルギー差({0.0_m, 20.0_m}).value, 0.01);
            Assert::AreEqual(
                1.10, g.下り勾配比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
        }

        TEST_METHOD(負の部分的勾配のグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, 0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, -0.01);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 30.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 15.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 40.0_m}).value, 0.0);
        }

        TEST_METHOD(正の部分的勾配のグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, -0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, 0.01);
            Assert::AreEqual(
                1.47, g.下り勾配比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
            Assert::AreEqual(
                0.74, g.下り勾配比エネルギー差({0.0_m, 15.0_m}).value, 0.01);
            Assert::AreEqual(
                1.47, g.下り勾配比エネルギー差({0.0_m, 40.0_m}).value, 0.01);
        }

        TEST_METHOD(負から正になる勾配のグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, 0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, -0.04);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 0.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 10.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 20.0_m}).value, 0.0);
            Assert::AreEqual(
                0.0, g.下り勾配比エネルギー差({0.0_m, 22.5_m}).value, 0.01);
            Assert::AreEqual(
                0.09, g.下り勾配比エネルギー差({0.0_m, 25.0_m}).value, 0.01);
            Assert::AreEqual(
                0.37, g.下り勾配比エネルギー差({0.0_m, 27.5_m}).value, 0.01);
            Assert::AreEqual(
                0.83, g.下り勾配比エネルギー差({0.0_m, 30.0_m}).value, 0.01);
            Assert::AreEqual(
                1.93, g.下り勾配比エネルギー差({0.0_m, 35.0_m}).value, 0.01);
        }

        TEST_METHOD(正から負になる勾配のグラフの下り勾配比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, -0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, 0.05);
            Assert::AreEqual(
                1.10, g.下り勾配比エネルギー差({0.0_m, 20.0_m}).value, 0.01);
            Assert::AreEqual(
                1.16, g.下り勾配比エネルギー差({0.0_m, 21.0_m}).value, 0.01);
            Assert::AreEqual(
                1.18, g.下り勾配比エネルギー差({0.0_m, 22.0_m}).value, 0.01);
            Assert::AreEqual(
                1.18, g.下り勾配比エネルギー差({0.0_m, 23.0_m}).value, 0.01);
            Assert::AreEqual(
                1.18, g.下り勾配比エネルギー差({0.0_m, 35.0_m}).value, 0.01);
        }

        TEST_METHOD(空のグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 0.0_m});
                Assert::AreEqual(0.0, z.value, 0.0);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.5_m, 1.0_m});
                Assert::AreEqual(1.0, z.value, 0.0);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
        }

        TEST_METHOD(負の勾配一つのグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 5.0_m});
                Assert::AreEqual(5.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }

            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 15.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 20.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 25.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }

            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 15.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 20.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 25.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }

            {
                auto [z, p] = g.最大比エネルギー差({12.5_m, 12.5_m});
                Assert::AreEqual(12.5, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({12.5_m, 15.0_m});
                Assert::AreEqual(12.5, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({12.5_m, 20.0_m});
                Assert::AreEqual(12.5, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({12.5_m, 25.0_m});
                Assert::AreEqual(12.5, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }

            {
                auto [z, p] = g.最大比エネルギー差({20.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({20.0_m, 25.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }

            {
                auto [z, p] = g.最大比エネルギー差({22.5_m, 25.0_m});
                Assert::AreEqual(22.5, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({25.0_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
        }

        TEST_METHOD(正の勾配一つのグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, -0.01);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 5.0_m});
                Assert::AreEqual(5.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }

            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 15.0_m});
                Assert::AreEqual(15.0, z.value, 0.001);
                auto e = g.比エネルギー差({9.0_m, 15.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({9.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({9.0_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                auto e = g.比エネルギー差({9.0_m, 25.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }

            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 15.0_m});
                Assert::AreEqual(15.0, z.value, 0.001);
                auto e = g.比エネルギー差({10.0_m, 15.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({10.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({10.0_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                auto e = g.比エネルギー差({10.0_m, 25.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }

            {
                auto [z, p] = g.最大比エネルギー差({15.0_m, 15.0_m});
                Assert::AreEqual(15.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({15.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({15.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({15.0_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                auto e = g.比エネルギー差({15.0_m, 25.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }

            {
                auto [z, p] = g.最大比エネルギー差({20.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({20.0_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                auto e = g.比エネルギー差({20.0_m, 25.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }

            {
                auto [z, p] = g.最大比エネルギー差({22.5_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                auto e = g.比エネルギー差({22.5_m, 25.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({25.0_m, 25.0_m});
                Assert::AreEqual(25.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
        }

        TEST_METHOD(負の部分的勾配のグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            g.勾配変化追加({30.0_m, 40.0_m}, -0.01);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 5.0_m});
                Assert::AreEqual(5.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 50.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
        }

        TEST_METHOD(正の部分的勾配のグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, -0.01);
            g.勾配変化追加({30.0_m, 40.0_m}, 0.01);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 5.0_m});
                Assert::AreEqual(5.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 5.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 10.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 50.0_m});
                Assert::AreEqual(50.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 50.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
        }

        TEST_METHOD(負から正になる勾配のグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({10.0_m, 20.0_m}, 0.01);
            g.勾配変化追加({30.0_m, 40.0_m}, -0.04);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 5.0_m});
                Assert::AreEqual(5.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 10.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 30.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 40.0_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 41.6_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                Assert::AreEqual(0.0, p.value, 0.0);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 41.7_m});
                Assert::AreEqual(41.7, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 41.7_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
        }

        TEST_METHOD(正から負になる勾配のグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, -0.01);
            g.勾配変化追加({20.0_m, 30.0_m}, 0.05);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 21.9_m});
                Assert::AreEqual(21.9, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 21.9_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 22.0_m});
                Assert::AreEqual(22.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 22.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 25.0_m});
                Assert::AreEqual(22.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 22.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 30.0_m});
                Assert::AreEqual(22.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 22.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 35.0_m});
                Assert::AreEqual(22.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 22.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
        }

        TEST_METHOD(最初の極大点が最大点であるグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 10.0_m}, -0.01);
            g.勾配変化追加({10.0_m, 30.0_m}, 0.02);
            g.勾配変化追加({30.0_m, 40.0_m}, -0.01);
            g.勾配変化追加({40.0_m, 45.0_m}, -0.005);
            g.勾配変化追加({45.0_m, 55.0_m}, 0.01);
            g.勾配変化追加({55.0_m, 60.0_m}, -0.005);
            g.勾配変化追加({60.0_m, 65.0_m}, -0.005);
            g.勾配変化追加({65.0_m, 75.0_m}, 0.01);
            g.勾配変化追加({75.0_m, 80.0_m}, -0.005);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 45.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 80.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 90.0_m});
                Assert::AreEqual(20.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 20.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
        }

        TEST_METHOD(最後の極大点が最大点であるグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 5.0_m}, -0.005);
            g.勾配変化追加({5.0_m, 15.0_m}, 0.01);
            g.勾配変化追加({15.0_m, 20.0_m}, -0.005);
            g.勾配変化追加({20.0_m, 25.0_m}, -0.005);
            g.勾配変化追加({25.0_m, 35.0_m}, 0.01);
            g.勾配変化追加({35.0_m, 40.0_m}, -0.005);
            g.勾配変化追加({40.0_m, 50.0_m}, -0.01);
            g.勾配変化追加({50.0_m, 70.0_m}, 0.02);
            g.勾配変化追加({70.0_m, 80.0_m}, -0.01);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 50.0_m});
                Assert::AreEqual(50.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 50.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 60.0_m});
                Assert::AreEqual(60.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 60.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 90.0_m});
                Assert::AreEqual(60.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 60.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
        }

        TEST_METHOD(等しい極大点を持つグラフの最大比エネルギー差)
        {
            勾配加速度グラフ g;
            g.勾配変化追加({0.0_m, 5.0_m}, -0.005);
            g.勾配変化追加({5.0_m, 15.0_m}, 0.01);
            g.勾配変化追加({15.0_m, 20.0_m}, -0.005);
            g.勾配変化追加({20.0_m, 25.0_m}, -0.005);
            g.勾配変化追加({25.0_m, 35.0_m}, 0.01);
            g.勾配変化追加({35.0_m, 40.0_m}, -0.005);
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 29.9_m});
                Assert::AreEqual(10.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 10.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
            {
                auto [z, p] = g.最大比エネルギー差({0.0_m, 30.1_m});
                Assert::AreEqual(30.0, z.value, 0.001);
                auto e = g.比エネルギー差({0.0_m, 30.0_m});
                Assert::AreEqual(e.value, p.value, 0.001);
            }
        }

    };
}
