// 減速目標.cpp : 制限速度を定め、それを維持するブレーキを計算します
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

#include "stdafx.h"
#include "減速目標.h"
#include <algorithm>
#include <cmath>
#include "共通状態.h"
#include "勾配グラフ.h"

namespace autopilot
{

    namespace
    {

        constexpr s 接近時間 = 2.0_s;
        constexpr double 減速度係数 = 0.15;

    }

#if 0
    運動状態 減速目標::パターン到達状態(mps 速度, 時刻 終端時刻) const
    {
        運動状態 走行{_位置, _速度, 終端時刻};
        if (速度 > _速度) {
            走行.指定速度まで走行(速度, -_基準減速度);
        }
        return 走行;

        // より正確なアルゴリズムは以下のようになるが、ここではそこまで
        // 正確さは求められていない
#if 0
        if (速度 <= _速度 + 速度マージン) {
            return 走行;
        }

        減速パターン パターン = 主パターン(平坦グラフ);

        if (速度 < パターン.通過速度()) {
            // 到達状態がブレーキを緩める途中の場合
            mps 速度差 = 速度 - _速度;
            double 速度倍率 = 速度差 / 速度マージン;
            s 時間差 = 接近時間 * std::log(速度倍率);
            m 距離 = _速度 * 時間差 + 速度差 * 接近時間;
            走行.変更(
                走行.位置() - 距離,
                走行.速度() + 速度差,
                走行.時刻() - 時間差);
            return 走行;
        }

        // パターンに当たる場所がもっと手前なら基準減速度で減速する
        走行.変更(パターン.通過地点(), パターン.通過速度(), -緩解時間);
        走行.指定速度まで走行(速度, -パターン.基準減速度());
        return 走行;
#endif
    }
#endif

    減速パターン 減速目標::主パターン(const 勾配グラフ &勾配) const
    {
        if (_速度 <= 0.0_mps) {
            return 減速パターン(_位置, 0.0_mps, _基準減速度);
        }
        if (_基準減速度 == mps2::無限大()) {
            return 減速パターン{_位置, _速度, _基準減速度};
        }

        // このクラスでは、基本的には 減速パターン クラスに従った等加速度運動
        // による減速を行う。ただし、速度が目標速度に近付いたところで緩やかに
        // ブレーキを緩めるため、目標速度に到達する直前の減速度は減速パターンの
        // 減速度より小さくなる。そのため、減速パターンの曲線がぴったり
        // 目標速度・目標位置を通過するようにするとわずかに速度を超えてしまう。
        // ブレーキを緩め始める瞬間の位置と速度を計算し、そこを通過するような
        // 減速パターンを用いることにする。

        // 微分方程式を解くと、ブレーキを緩めてゆく運動は指数関数の式になる
        // ことが分かる。ブレーキを緩め始める地点はこの指数関数が等加速度運動の
        // パターン (二次関数) と接する点になるので、以下「接点」と呼ぶ。

        // まず目標位置における勾配加速度を求める。
        // 下り勾配ではこの分だけ基準減速度から差し引く。
        mps2 勾配加速度 = 勾配.列車勾配加速度(_位置);

        // 接点での減速度を求める
        constexpr mps2 接点減速度下限 = 速度マージン / 接近時間;
        mps2 接点減速度 = std::max(
            _基準減速度 - std::max(勾配加速度, 0.0_mps2), 接点減速度下限);

        // 接点での速度が目標速度よりどれだけ高いかを求める
        mps 接点速度差 = 接点減速度 * 接近時間;

        // ブレーキを緩めるのにかかる時間(の上界)を求める。つまり、減速度が
        // 接点減速度から 接点減速度下限 以下まで下がる時間。
        // (微分方程式を解くと、減速度は 1 秒当たり exp(-1 / 接近時間) 倍に
        // 小さくなることが分かる。そこからこのような計算式になる)
        double 減速度倍率 = 接点減速度 / 接点減速度下限;
        s 緩解時間 = 接近時間 * std::log(減速度倍率);

        // 緩解時間の間に列車が進む距離を求める
        m 緩解距離 = _速度 * 緩解時間 + 接点速度差 * 接近時間;

        // 接点を通る減速パターンを作る
        return 減速パターン(_位置 - 緩解距離, _速度 + 接点速度差, _基準減速度);
    }

    mps2 減速目標::副パターン減速度(const 運動状態 &運動状態) const
    {
        constexpr const 勾配グラフ &勾配 = 勾配グラフ::平坦グラフ;

        mps2 基準減速度 =
            static_cast<mps2>(std::sqrt(運動状態.速度().value * 減速度係数));
        減速パターン パターン{_位置, _速度, 基準減速度};
        mps 期待速度 = パターン.期待速度(運動状態.位置(), 勾配);
        mps2 期待減速度 = パターン.期待減速度(運動状態.位置(), 勾配);
        mps2 出力減速度 =
            期待減速度 * (運動状態.速度() / 期待速度) +
            (運動状態.速度() - 期待速度) / 接近時間;
        return 出力減速度;
    }

    mps2 減速目標::減速用出力減速度(
        const 運動状態 &運動状態, const 勾配グラフ &勾配) const
    {
        減速パターン パターン1 = 主パターン(勾配);
        mps 期待速度 = パターン1.期待速度(運動状態.位置(), 勾配);

        if (期待速度 == 0.0_mps) {
            return mps2::無限大();
        }
        if (運動状態.速度() > 期待速度 && 運動状態.位置() < _位置) {
            // 目標位置までの途中でパターンに戻れるような減速度にする
            m 中間地点 = (_位置 + 運動状態.位置()) / 2.0;
            mps 中間速度 = パターン1.期待速度(中間地点, 勾配);
            減速パターン パターン2 = 減速パターン::二点間パターン(
                中間地点, 中間速度, 運動状態.位置(), 運動状態.速度(), 勾配);
            mps2 出力減速度 = パターン2.期待減速度(運動状態.位置(), 勾配);
            return 出力減速度;
        }

        // 期待(減)速度に漸次的に近付けるように減速度を調整する。
        // 基本的には 2 秒後に期待速度に到達するような減速度を出力する。
        // ただし現在速度が期待速度と異なることにより
        // 期待速度の実際の減速度も期待減速度とは異なるので
        // 期待減速度に現在速度と期待速度の比を掛けて補正する。
        mps2 期待減速度 = パターン1.期待減速度(運動状態.位置(), 勾配);
        mps2 出力減速度1 =
            期待減速度 * (運動状態.速度() / 期待速度) +
            (運動状態.速度() - 期待速度) / 接近時間;

        if (isnan(出力減速度1)) {
            return 0.0_mps2;
        }
        if (_速度 > 0.0_mps || 運動状態.位置() >= _位置) {
            return 出力減速度1;
        }

        // 初速が低いときはやや早めに減速し始める
        mps2 出力減速度2 = 副パターン減速度(運動状態);

        // 手前に止まってしまうような強い減速は避ける
        // (停止直前に「あと 2 秒で期待速度に到達」では間に合わない)
        減速パターン パターン3 = 減速パターン::二点間パターン(
            _位置, 0.0_mps, 運動状態.位置(), 運動状態.速度(), 勾配);
        mps2 出力減速度3 = パターン3.期待減速度(運動状態.位置(), 勾配);
        return std::min(std::max(出力減速度1, 出力減速度2), 出力減速度3);
    }

    mps2 減速目標::収束用出力減速度(mps 現在速度) const
    {
        if (_速度 <= 0.0_mps) {
            return mps2::無限大();
        }
        return (現在速度 - _速度) / 接近時間;
    }

    自動制動自然数ノッチ 減速目標::出力制動ノッチ(
        const 運動状態 &運動状態, const 共通状態 &状態) const
    {
        const 制動特性 &制動 = 状態.制動();
        mps2 出力減速度1 = 減速用出力減速度(運動状態, 状態.勾配());
        mps2 出力減速度2 = 収束用出力減速度(運動状態.速度());
        mps2 勾配加速度 = 状態.勾配().列車勾配加速度(運動状態.位置());
        mps2 補正出力減速度1 = 出力減速度1 + 勾配加速度;
        mps2 補正出力減速度2 = 出力減速度2 + std::max(勾配加速度, 0.0_mps2);
        自動制動実数ノッチ 出力実数1 = 制動.自動ノッチ(補正出力減速度1);
        自動制動実数ノッチ 出力実数2 = 制動.自動ノッチ(補正出力減速度2);
        // TODO ノッチの頻繁な上下を避けるためにここで多少のバイアスを入れる
        自動制動自然数ノッチ 出力自然数1 = 制動.自動ノッチ丸め(出力実数1);
        自動制動自然数ノッチ 出力自然数2 = 出力実数2.ceil();
        return std::min({出力自然数1, 出力自然数2, 制動.自動最大ノッチ()});
    }

    std::pair<mps, 区間> 減速目標::最低許容速度区間(区間 範囲) const
    {
        m パターン境界;

        if (_基準減速度 == mps2::無限大()) {
            パターン境界 = _位置;
        }
        else {
            // ここでは平坦グラフで近似しても特に大きな問題はない
            減速パターン パターン = 主パターン(勾配グラフ::平坦グラフ);
            // パターンぴったりよりも多少早くブレーキをかけ始めるので
            // 境界点を少し手前にしておく
            パターン境界 =
                パターン.通過地点() - パターン.通過速度() * 接近時間;
        }

        区間 最低速度区間{
            std::min(std::max(パターン境界, 範囲.始点), 範囲.終点),
            範囲.終点};
        mps 最低速度 = 最低速度区間.真区間である() ? _速度 : mps::無限大();
        return {最低速度, 最低速度区間};
    }

}
