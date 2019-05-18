// 走行モデル.cpp : 走行列車の位置・速度・時刻をシミュレートします。
//
// Copyright © 2019 Watanabe, Yuki
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

#include "stdafx.h"
#include "走行モデル.h"
#include <cmath>

namespace autopilot {

    加速度型 走行モデル::指定時間走行(
        時間型 時間, 加速度型 初加速度, 加加速度型 加加速度)
    {
        _位置 += 時間 * (_速度 + 時間 * (初加速度 + 時間 * 加加速度 / 3) / 2);
        _速度 += 時間 * (初加速度 + 時間 * 加加速度 / 2);
        _時刻 += 時間;
        return 初加速度 + 時間 * 加加速度;
    }

    加速度型 走行モデル::指定時刻まで走行(
        時間型 時刻, 加速度型 初加速度, 加加速度型 加加速度)
    {
        加速度型 終加速度 = 指定時間走行(時刻 - _時刻, 初加速度, 加加速度);
        _時刻 = 時刻; // 誤差をなくすため直接再代入する
        return 終加速度;
    }

    void 走行モデル::指定距離走行(距離型 距離, 加速度型 加速度)
    {
        if (距離 == 0) {
            return;
        }
        if (加速度 == 0) {
            _時刻 += 距離 / _速度;
            _位置 += 距離;
            return;
        }

        速度型 新速度 = std::sqrt(_速度 * _速度 + 2 * 距離 * 加速度);
        if (!(新速度 >= 0)) {
            新速度 = 0;
        }
        _時刻 += (新速度 - _速度) / 加速度;
        _速度 = 新速度;
        _位置 += 距離;
    }

    void 走行モデル::指定位置まで走行(距離型 位置, 加速度型 加速度)
    {
        指定距離走行(位置 - _位置, 加速度);
        _位置 = 位置; // 誤差をなくすため直接再代入する
    }

    void 走行モデル::指定速度まで走行(速度型 速度, 加速度型 加速度)
    {
        if (速度 == _速度) {
            return;
        }
        指定時間走行((速度 - _速度) / 加速度, 加速度);
        _速度 = 速度; // 誤差をなくすため直接再代入する
    }

    加速度型 走行モデル::指定位置まで走行(
        距離型 位置, 加速度型 初加速度, 加加速度型 加加速度)
    {
        // 三次方程式を代数的に解くのは面倒なのでニュートン法を使う。
        // 実数解が複数あるときどの解に行きつくかは分からない。
        走行モデル tmp = *this;
        加速度型 加速度 = 初加速度;
        for (size_t i = 0; i < 10; i++) {
            tmp.指定位置まで走行(位置, 加速度);
            時間型 時刻 = tmp.時刻();
            tmp = *this;
            加速度 = tmp.指定時刻まで走行(時刻, 初加速度, 加加速度);
        }

        *this = tmp;
        _位置 = 位置; // 誤差をなくすため直接再代入する
        return 加速度;
    }

    void 走行モデル::等加加速度で指定加速度まで走行(
        加速度型 初加速度, 加速度型 終加速度, 加加速度型 加加速度)
    {
        時間型 時間 = (終加速度 - 初加速度) / 加加速度;
        指定時間走行(時間, 初加速度, 加加速度);
    }

    加速度型 走行モデル::距離と速度による加速度(
        距離型 距離, 速度型 初速度, 速度型 終速度)
    {
        return (終速度 * 終速度 - 初速度 * 初速度) / 距離 / 2.0;
    }

}
