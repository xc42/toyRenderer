#pragma once

#include <cmath>
//#include <vector>
#include <limits>
#include <algorithm>

template<int N, typename T>
struct Vec {
    T data_[N];

    Vec()=default;

    Vec(const std::initializer_list<T>& v) { std::copy_n(std::cbegin(v), N, data_); }

    Vec(const Vec<N-1, T>& v, T last = T{}) {
        std::copy(v.data_, v.data_ + N - 1, data_);
        data_[N-1] = last;
    }

    T  operator[](int idx) const{ return data_[idx]; }
    T& operator[](int idx) { return data_[idx]; }

#define vec_op_scalar(OP) \
        Vec<N, decltype(s OP data_[0])> ans; \
        for(int i = 0; i < N; ++i) ans[i] = data_[i] OP s; \
        return ans;

    template<typename OT>
    Vec operator+(const OT& s) const{ vec_op_scalar(+) }
    template<typename OT>
    Vec operator-(const OT& s) const{ vec_op_scalar(-) }
    template<typename OT>
    Vec operator*(const OT& s) const{ vec_op_scalar(*) }
    template<typename OT>
    Vec operator/(const OT& s) const{ vec_op_scalar(/) }

#define vec_op_vec(OP) \
    Vec<N, decltype(v[0] OP data_[0])> ans; \
    for(int i = 0; i < N; ++i) ans[i] = data_[i] OP v[i]; \
    return ans;

    template<typename OT>
    Vec operator+(const Vec<N, OT>& v) const{ vec_op_vec(+) }
    template<typename OT>
    Vec operator-(const Vec<N, OT>& v) const{ vec_op_vec(-) }
    template<typename OT>
    Vec operator*(const Vec<N, OT>& v) const{ vec_op_vec(*) }
    template<typename OT>
    Vec operator/(const Vec<N, OT>& v) const{ vec_op_vec(/) }

#define vec_op_scalar_inplace(OP) \
    for(int i = 0; i < N; ++i) data_[i] OP s; \
    return *this;

    Vec& operator+=(const T s) { vec_op_scalar_inplace(+=) }
    Vec& operator-=(const T s) { vec_op_scalar_inplace(-=) }
    Vec& operator*=(const T s) { vec_op_scalar_inplace(*=) }
    Vec& operator/=(const T s) { vec_op_scalar_inplace(/=) }

#define vec_op_vec_inplace(OP) \
    for(int i = 0; i < N; ++i) data_[i] OP v[i]; \
    return *this;

    Vec operator+=(const Vec<N, T>& v) { vec_op_vec_inplace(+=) }
    Vec operator-=(const Vec<N, T>& v) { vec_op_vec_inplace(-=) }
    Vec operator*=(const Vec<N, T>& v) { vec_op_vec_inplace(*=) }
    Vec operator/=(const Vec<N, T>& v) { vec_op_vec_inplace(/=) }

    T* begin() { return data_; }
    T* end() { return data_ + N; }

    template<typename OT>
    auto  dot_product(const Vec<N, OT> & v) {
        decltype(OT()*T()) ans{};
        for(int i = 0; i < N; ++i) 
            ans += data_[i] * v[i];
        return ans;
    }

    Vec<N, float> normal() {
        T sum{};
        for(int i = 0; i < N; ++i) sum += data_[i] * data_[i];
        return (*this) / sqrtf(static_cast<float>(sum));
    }

};

template<typename T>
using Vec2 = Vec<2, T>;
template<typename T>
using Vec3 = Vec<3, T>;
template<typename T>
using Vec4 = Vec<4, T>;

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Vec4f = Vec4<float>;
using Vec4i = Vec4<int>;

template<typename T>
using Point2 = Vec2<T>;

using Point2f = Vec2f;
using Point2i = Vec2i;
using Point3f = Vec3f;
using Point3i = Vec3i;

template<typename T>
Vec3<T> CrossProduct(const Vec3<T> &v1, const Vec3<T> &v2) {
    return {v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]};
}

template<typename T>
struct Rect {
    Rect()=default;
    Rect(const Vec<2,T>& l, const Vec<2,T>& r):lb(l), rt(r){}
    Vec<2,T> lb, rt;
};

template<int N, int M, typename T>
struct Matrix {
    using RowType = Vec<M, T>;
    using ColType = Vec<N, T>;
    enum class Stacking {ByRow, ByColumn};

    RowType data_[N];

    Matrix()=default;

private:
    Matrix(const std::initializer_list<RowType>& rows, float* tag=nullptr) { 
        std::copy_n(rows.begin(), N, data_); 
    }

    Matrix(const std::initializer_list<ColType>& cols, int* tag=nullptr) {
        auto pcol = cols.begin();
        for(int i = 0; i < M; ++i, ++pcol) {
            for(int j = 0; j < N; ++j) {
                data_[i][j] = (*pcol)[j];
            }
        }
    }

public:

    template<int D, typename TAG = std::enable_if_t<D == N || D == M>>
    Matrix(const std::initializer_list<Vec<D, T>>& vecs, Stacking type = Stacking::ByRow) {
        if constexpr (M != N) {
            Matrix(vecs);
        } else {
            type == Stacking::ByRow? Matrix(vecs, (float*)0): Matrix(vecs, (int*)0);
        }
    }

    RowType* begin() { return data_; }
    RowType* end() { return data_ + N; }

    RowType& operator[](const int idx) { return data_[idx]; }

    Vec<N, T> operator*(const Vec<M, T>& v) const{
        Vec<N, T> ans;
        for(int i = 0; i < N; ++i) ans[i] = data_[i].dot_product(v);
        return ans;
    }

    template<int D>
    Matrix<N, D, T> operator*(const Matrix<M, D, T>& m) const{
        Matrix<N, D, T> ans;
        for(int i = 0; i < N; ++i)
            for(int j = 0; j < D; ++j) {
                ans[i][j] = 0;
                for(int k = 0; k < M; ++k) 
                    ans[i][j] += data_[i][k] * m.data_[k][j];
            }
        return ans;
    }

    T det() { 
        if constexpr(N == 2 && M == 2) {
            return data_[0][0] * data_[1][1] - data_[0][1] * data_[1][0];
        } else {
            //unimplement
        }
    }

};

template<typename T>
using Matrix2x2 = Matrix<2, 2, T>;

template<typename T>
using Matrix3x3 = Matrix<3, 3, T>;

template<typename T>
using Matrix4x4 = Matrix<4, 4, T>;

using Matrix3x3f = Matrix3x3<float>;
using Matrix4x4f = Matrix4x4<float>;

//template<typename T>
//using Array2D = std::vector<std::vector<T>>;

