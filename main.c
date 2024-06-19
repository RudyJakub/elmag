/*
    Program oblicza ruch naładowanej cząteczki w 
    stałym polu elektromagnetycznym.

*/

#include <stdio.h>

enum Constants {
    COULOMBS_CONSTANT = 89, // * 10^8 [m^2/C^2]
    ELECTRON_MASS = 91, // * 10^-31 [kg]
    ELEMENTARY_CHARGE = 16 // * 10^-19 [C]
};

// Stukrura opisująca wektor o składowych (x,y,z).
typedef struct Vector {
    double x;
    double y;
    double z;
} Vector;

// Zwraca wektor przemnożony przez skalar.
Vector vectorMultiply(Vector v, float scalar) {
    Vector u = (Vector){
        v.x * scalar,
        v.y * scalar,
        v.z * scalar
    };
    return u;
}

void printVector(Vector v) {
    printf("(%f,%f,%f)\n", v.x, v.y, v.z);
}

// Struktura opisująca macierz 3x3 za pomocą rzędów wektorów.
typedef struct Matrix {
    Vector row1;
    Vector row2;
    Vector row3;
} Matrix;

// Struktura opisująca naładowaną cząsteczkę.
typedef struct ChargedParticle {
    float charge; // Ładunek
    float mass; // Masa cząsteczki
    Vector pos; // Przemieszczenie - Wektor (x,y,z)
    Vector vel; // Prędkość - Wektor (x,y,z)
    Vector acc; // Przyspieszenie - Wektor (x,y,z)
} ChargedParticle;

// F = ma
// v = v0 * at
// x = x0 + vt
void updateChargedParticle(ChargedParticle *particle, Vector force, float dt) {
    particle->acc = (Vector){
        force.x / particle->mass,
        force.y / particle->mass,
        force.z / particle->mass
    };
    particle->vel = (Vector){
        particle->vel.x + particle->acc.x * dt,
        particle->vel.y + particle->acc.y * dt,
        particle->vel.z + particle->acc.z * dt
    };
    particle->pos = (Vector){
        particle->pos.x + particle->vel.x * dt,
        particle->pos.y + particle->vel.y * dt,
        particle->pos.z + particle->vel.z * dt
    };
}

/*
    | i   j   k   |
    | qvx qvy qvz | = qv x B
    | Bx  By  Bz  |

*/
// F = qv x B
Vector computeLorentzForce(ChargedParticle particle, Vector field) {
    Vector force = (Vector){
        (particle.vel.y * field.z - particle.vel.z * field.y),
        (particle.vel.z * field.x - particle.vel.x * field.z),
        (particle.vel.x * field.y - particle.vel.y * field.x)
    };
    force = vectorMultiply(force, particle.charge);
    return force;
}

// F = Eq
Vector computeCoulombsForce(ChargedParticle particle, Vector field) {
    Vector force = vectorMultiply(field, particle.charge);
    return force;
}

int main() {
    // Stałe pole indukcji magneycznej.
    Vector magneticField = (Vector){ 0, 0, 10 }; // T
    // Stałe pole natężenia elektrycznego.
    Vector electricField = (Vector){ 10, 10, 5 }; // [N/C]

    ChargedParticle particle = (ChargedParticle){
        ELEMENTARY_CHARGE / 10,
        ELECTRON_MASS / 10,
        (Vector){ 0, 0, 0 }, // Początkowe położenie [mm]
        (Vector){ 0, 0, 0 }, // Począrkowa prędkość [mm/ns]
        (Vector){ 0, 0, 0 }  // Początkowe przyspieszenie [mm/ns^2]
    };

    float time_total = 10; // Całkowity czas lotu cząsteczki [10^-9 s - nanosekunda]
    float dt = time_total / 100;

    // Tablice z wynikami
    Vector positions[101];
    float t[101];


    int i = 0;
    // Co zmianę czasu dt liczy przyspieszenie cząsteczki oraz jest nowe położenie i prędkość,
    // następnie wyniki zapisywane w tablicach.
    for (float time_elapsed=0; time_elapsed<time_total; time_elapsed = time_elapsed + dt) {
        positions[i] = particle.pos;
        t[i] = time_elapsed;
        Vector coulombsForce = computeCoulombsForce(particle, electricField);
        Vector lorentzForce = computeLorentzForce(particle, magneticField);
        // Siła wypadkowa:
        Vector netForce = (Vector){
            coulombsForce.x + lorentzForce.x,
            coulombsForce.y + lorentzForce.y,
            coulombsForce.z + lorentzForce.z
        };
        updateChargedParticle(&particle, netForce, dt);
        i++;
    }

    // Wypisanie wyników:
    for (int j=0; j<i; j++) {
        printf("TIME[ns]: %.2f,   POS[mm]: (%.2f, %.2f, %.2f) \n", t[j], positions[j].x, positions[j].y, positions[j].z);
    }

    return 0;
}
