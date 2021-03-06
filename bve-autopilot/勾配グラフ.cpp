// 勾配グラフ.cpp : 勾配による列車の挙動への影響を計算します
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
#include "勾配グラフ.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <utility>
#include "区間.h"

#pragma warning(disable:4819)

namespace autopilot
{

    namespace
    {

        constexpr mps2 重力加速度 = 9.80665_mps2;

        constexpr mps2 勾配加速度(勾配グラフ::勾配 勾配) noexcept
        {
            return -0.75 * 重力加速度 * 勾配;
        }

    }

    mps2 勾配加速度グラフ::勾配加速度(m 位置) const
    {
        return 勾配加速度(_変化点リスト.upper_bound(位置), 位置);
    }

    /// 引数区間の始点を始点とし終点が引数区間の中に含まれているような区間
    /// のうち、比エネルギー差が最大となるものを求め、
    /// その終点と比エネルギー差を返す
    std::pair<m, m2ps2> 勾配加速度グラフ::最大比エネルギー差(区間 区間) const
    {
        assert(!区間.空である());

        const_iterator i = _変化点リスト.upper_bound(区間.始点);
        m2ps2 初期比エネルギー = 比エネルギー(i, 区間.始点);

        std::pair<m, m2ps2> 最大点{区間.始点, 0.0_m2ps2};

        m z1 = 区間.始点;
        mps2 a1 = 0.0_mps2;
        if (i != _変化点リスト.begin()) {
            const_iterator h = std::prev(i);
            z1 = h->first;
            a1 = h->second.勾配加速度;
        }

        // 比エネルギー差が最大となる地点は勾配加速度が正から負に変化するところ
        // であるから、そのような地点を探す
        for (; i != _変化点リスト.end(); ++i) {
            m z2 = i->first;
            mps2 a2 = i->second.勾配加速度;

            if (a1 >= 0.0_mps2 && a2 < 0.0_mps2) {
                double 比 = a1 / (a1 - a2);
                m 極大位置 = z1 + 比 * (z2 - z1);
                if (区間.含む(極大位置)) {
                    m2ps2 極大比エネルギー =
                        比エネルギー(i, 極大位置) - 初期比エネルギー;
                    if (極大比エネルギー >= 最大点.second) {
                        最大点.first = 極大位置;
                        最大点.second = 極大比エネルギー;
                    }
                }
            }

            if (z2 > 区間.終点) {
                break;
            }

            z1 = z2;
            a1 = a2;
        }

        assert(i == _変化点リスト.upper_bound(区間.終点));

        // 区間.終点 において勾配加速度が正なら極大である
        m2ps2 終点比エネルギー = 比エネルギー(i, 区間.終点) - 初期比エネルギー;
        if (終点比エネルギー >= 最大点.second) {
            最大点.first = 区間.終点;
            最大点.second = 終点比エネルギー;
        }

        return 最大点;
    }

    void 勾配加速度グラフ::勾配変化追加(区間 変化区間, 勾配 勾配変化量)
    {
        if (変化区間.始点 >= 変化区間.終点) {
            throw std::invalid_argument("non-empty interval required");
        }

        // まず始点の値を設定する
        const_iterator h = _変化点リスト.upper_bound(変化区間.始点);
        iterator i = _変化点リスト.try_emplace(
            h, 変化区間.始点, 勾配加速度(h, 変化区間.始点));

        if (_累積比エネルギー未計算位置 == _変化点リスト.end() ||
            _累積比エネルギー未計算位置->first > 変化区間.始点)
        {
            _累積比エネルギー未計算位置 = i;
        }
        if (_累積下り勾配比エネルギー未計算位置 == _変化点リスト.end() ||
            _累積下り勾配比エネルギー未計算位置->first > 変化区間.始点)
        {
            _累積下り勾配比エネルギー未計算位置 = i;
        }

        // 終点の値を求める
        mps2 加速度変化量 = autopilot::勾配加速度(勾配変化量);
        mps2 新しい終点加速度 = 勾配加速度(変化区間.終点) + 加速度変化量;

        // 始点より後にある点を更新する
        m 区間長さ = 変化区間.長さ();
        while (++i != _変化点リスト.end()) {
            double 比 = std::min((i->first - 変化区間.始点) / 区間長さ, 1.0);
            i->second.勾配加速度 += 比 * 加速度変化量;
            i->second.累積比エネルギー = m2ps2::quiet_NaN();
            i->second.累積下り勾配比エネルギー = m2ps2::quiet_NaN();
        }

        // 終点の値を設定する
        _変化点リスト.insert_or_assign(
            _変化点リスト.end(), 変化区間.終点, 変化点{新しい終点加速度});
    }

    m2ps2 勾配加速度グラフ::比エネルギー差(mps2 a2, mps2 a1, m 変位)
    {
        return 0.5 * 変位 * (a1 + a2);
    }

    m2ps2 勾配加速度グラフ::下り勾配比エネルギー差(mps2 a2, mps2 a1, m 変位)
    {
        if (a2 <= 0.0_mps2) {
            if (a1 <= 0.0_mps2) {
                return 0.0_m2ps2;
            }
            double 比 = a1 / (a1 - a2);
            return 0.5 * 比 * 変位 * a1;
        }
        if (a1 >= 0.0_mps2) {
            return 0.5 * 変位 * (a1 + a2);
        }
        double 比 = a2 / (a2 - a1);
        return 0.5 * 比 * 変位 * a2;
    }

    mps2 勾配加速度グラフ::勾配加速度(const_iterator i, m 位置) const
    {
        if (i == _変化点リスト.begin()) {
            return 0.0_mps2;
        }
        const_iterator h = std::prev(i);
        if (i == _変化点リスト.end()) {
            return h->second.勾配加速度;
        }
        m h位置 = h->first, i位置 = i->first;
        assert(h位置 <= 位置);
        assert(位置 <= i位置);
        double 比 = (位置 - h位置) / (i位置 - h位置);
        return h->second.勾配加速度 +
            比 * (i->second.勾配加速度 - h->second.勾配加速度);
    }

    m2ps2 勾配加速度グラフ::比エネルギー(const_iterator i, m 位置) const
    {
        if (i == _変化点リスト.begin()) {
            return 0.0_m2ps2;
        }
        const_iterator h = std::prev(i);

        if (isnan(h->second.累積比エネルギー)) {
            // 累積比エネルギーが未計算なので計算する
            const_iterator p = _累積比エネルギー未計算位置;
            if (p == _変化点リスト.begin()) {
                p->second.累積比エネルギー = 0.0_m2ps2;
                ++_累積比エネルギー未計算位置;
            }
            else {
                --p;
            }

            for (; _累積比エネルギー未計算位置 != i;
                ++p, ++_累積比エネルギー未計算位置)
            {
                auto &[z1, p1] = *p;
                auto &[z2, p2] = *_累積比エネルギー未計算位置;
                p2.累積比エネルギー = p1.累積比エネルギー +
                    比エネルギー差(p2.勾配加速度, p1.勾配加速度, z2 - z1);
            }
        }

        assert(!isnan(h->second.累積比エネルギー));
        mps2 a2 = 勾配加速度(i, 位置);
        m dz = 位置 - h->first;
        return h->second.累積比エネルギー +
            比エネルギー差(a2, h->second.勾配加速度, dz);
    }

    m2ps2 勾配加速度グラフ::比エネルギー(m 位置) const
    {
        return 比エネルギー(_変化点リスト.upper_bound(位置), 位置);
    }

    m2ps2 勾配加速度グラフ::下り勾配比エネルギー(m 位置) const
    {
        const_iterator i = _変化点リスト.upper_bound(位置);
        if (i == _変化点リスト.begin()) {
            return 0.0_m2ps2;
        }
        const_iterator h = std::prev(i);

        if (isnan(h->second.累積下り勾配比エネルギー)) {
            // 累積比エネルギーが未計算なので計算する
            const_iterator p = _累積下り勾配比エネルギー未計算位置;
            if (p == _変化点リスト.begin()) {
                p->second.累積下り勾配比エネルギー = 0.0_m2ps2;
                ++_累積下り勾配比エネルギー未計算位置;
            }
            else {
                --p;
            }

            for (; _累積下り勾配比エネルギー未計算位置 != i;
                ++p, ++_累積下り勾配比エネルギー未計算位置)
            {
                auto &[z1, p1] = *p;
                auto &[z2, p2] = *_累積下り勾配比エネルギー未計算位置;
                p2.累積下り勾配比エネルギー = p1.累積下り勾配比エネルギー +
                    下り勾配比エネルギー差(
                        p2.勾配加速度, p1.勾配加速度, z2 - z1);
            }
        }

        assert(!isnan(h->second.累積下り勾配比エネルギー));
        mps2 a2 = 勾配加速度(i, 位置);
        m dz = 位置 - h->first;
        return h->second.累積下り勾配比エネルギー +
            下り勾配比エネルギー差(a2, h->second.勾配加速度, dz);
    }

    勾配グラフ::勾配グラフ() = default;
    勾配グラフ::~勾配グラフ() = default;

    const 勾配グラフ 勾配グラフ::平坦グラフ{};

    void 勾配グラフ::消去() noexcept
    {
        _区間リスト.clear();
        _加速度キャッシュ.clear();
    }

    void 勾配グラフ::列車長を設定(m 列車長) noexcept
    {
        // 勾配加速度グラフは長さ 0 の列車を扱えないので 0 にはしない
        _列車長 = std::max(列車長, 0.5_m);

        _加速度キャッシュ.clear();
    }

    void 勾配グラフ::勾配区間追加(m 始点, 勾配 勾配)
    {
        _区間リスト.insert_or_assign(始点, 勾配);

        _加速度キャッシュ.clear();
    }

    void 勾配グラフ::通過(m 列車先頭位置)
    {
        if (_区間リスト.empty()) {
            return;
        }

        // 通過済みの区間を消す
        m 列車最後尾位置 = 列車先頭位置 - _列車長;
        auto i = _区間リスト.begin();
        while (true) {
            auto j = std::next(i);
            if (j == _区間リスト.end() || j->first > 列車最後尾位置) {
                break;
            }
            i = j;
        }
        if (i == _区間リスト.begin()) {
            return;
        }
        i = _区間リスト.erase(_区間リスト.begin(), i);
        assert(!_区間リスト.empty());
        assert(i == _区間リスト.begin());

        _加速度キャッシュ.clear();
    }

    mps2 勾配グラフ::列車勾配加速度(m 列車先頭位置) const
    {
        加速度キャッシュ構築();
        return _加速度キャッシュ.勾配加速度(列車先頭位置);
    }

    m2ps2 勾配グラフ::比エネルギー差(区間 変位) const
    {
        加速度キャッシュ構築();
        return _加速度キャッシュ.比エネルギー差(変位);
    }

    m2ps2 勾配グラフ::下り勾配比エネルギー差(区間 変位) const
    {
        加速度キャッシュ構築();
        return _加速度キャッシュ.下り勾配比エネルギー差(変位);
    }

    std::pair<m, m2ps2> 勾配グラフ::最大比エネルギー差(区間 区間) const
    {
        加速度キャッシュ構築();
        return _加速度キャッシュ.最大比エネルギー差(区間);
    }

    void 勾配グラフ::加速度キャッシュ構築() const
    {
        if (!_加速度キャッシュ.empty()) {
            return;
        }

        勾配 前の勾配 = 0.0;
        for (auto &[位置, 勾配] : _区間リスト) {
            _加速度キャッシュ.勾配変化追加(
                {位置, 位置 + _列車長}, 勾配 - 前の勾配);
            前の勾配 = 勾配;
        }
    }

}
