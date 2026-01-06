#pragma once
#include <cstdint>
#include <iostream>
#include <concepts>
#include <algorithm>

// Helper mágico para calcular potencias en tiempo de compilación
consteval int64_t power(int64_t base, int exp) {
    int64_t res = 1;
    for (int i = 0; i < exp; ++i) res *= base;
    return res;
}

template<int Decimals>
class Fixed {
    // Variable interna: Para 10.50 con Decimals=2, esto vale 1050
    int64_t raw_value;

public:
    static constexpr int64_t Scale = power(10, Decimals);

    // Constructor por defecto
    constexpr Fixed() : raw_value(0) {}

    // Constructor desde entero (Ej: Fixed(10) -> 1000)
    constexpr Fixed(int64_t v) : raw_value(v * Scale) {}

    // Constructor desde float (Ej: Fixed(10.5f) -> 1050)
    constexpr Fixed(double v) : raw_value(static_cast<int64_t>(v * Scale)) {}

    // Constructor "raw" (privado o protegido idealmente, pero público para el operador)
    // Lo usamos cuando ya tenemos el valor escalado
    static constexpr Fixed from_raw(int64_t raw) {
        Fixed f;
        f.raw_value = raw;
        return f;
    }

    // --- OPERADORES ---

    // 1. Suma (Fácil)
    template<int OtherDecimals>
    constexpr auto operator+(const Fixed& other) const {
      constexpr int64_t ResultDecimals = std::max(Decimals, OtherDecimals);

      using ResultType = Fixed<ResultDecimals>;

      int64_t my_val = raw_value;
      int64_t other_val = other.raw_value;

      if constexpr(Decimals < ResultDecimals){
        constexpr int64_t diff_scale = power(10, ResultDecimals - Decimals);
        my_scaled *= diff_scale;
      }
      else if constexpr(OtherDecimals < ResultDecimals) {
        constexpr int64_t diff_scale = power(10, ResultDecimals - OtherDecimals);
        other_scaled *= diff_scale;
      }

      return ResultType::from_raw(my_val + other_val);
    }

    // 2. Resta (Fácil)
    // TODO


    // 3. Multiplicación (Difícil)
    constexpr Fixed operator*(const Fixed& other) const {
        // TODO: Cuidado con el overflow aquí. 
        // Lo ideal es castear a __int128_t temporalmente si el compilador lo soporta,
        // o simplemente hacerlo con int64_t asumiendo que no desborda para este ejercicio.
        int64_t res = (this->raw_value * other.raw_value) / Scale;
        return from_raw(res);
    }

    // 4. División (Difícil)
    // TODO: Implementar operator/

    // Getter para debug
    double to_double() const {
        return static_cast<double>(raw_value) / Scale;
    }
};

// --- LITERAL PARA EL GITHUB ---
// Esto permite escribir 19.99_USD
constexpr Fixed<2> operator""_USD(long double val) {
    return Fixed<2>(static_cast<double>(val));
}
