using namespace std;

#include "Interfaces.cpp"

#ifndef COMPLEX_NUMBER_H // include guard
#define COMPLEX_NUMBER_H
class ComplexNumber : public IComplexNumber {
    // Constructors
    public:
        ComplexNumber() {
            imaginary = 0;
            real = 1;
        }

        ComplexNumber(long real) {
            this->real = real;
            imaginary = 0;
            this->normalize();
        }

        ComplexNumber(long real, long imaginary) {
            this->imaginary = imaginary;
            this->real = real;
            this->normalize();
        }

    // Methods
    public:
        long getRealPart() {
            return this->real;
        }

        long getImaginaryPart() {
            return this->imaginary;
        }

        string get_string() {
            return std::to_string(this->real) + "+" + std::to_string(this->imaginary) + "i";
        }

        void normalize() {
            //cout << this->get_string() << "\n";
            this->real = this->real % MOD_NUMBER;
            this->imaginary = this->imaginary % MOD_NUMBER;
        }

        IComplexNumber* product(IComplexNumber* cx) {
            long newReal = this->real * cx->getRealPart() - this->imaginary * cx->getImaginaryPart();
            long newImaginary = this->real * cx->getImaginaryPart() + this->imaginary * cx->getRealPart();
            return new ComplexNumber(newReal, newImaginary);
        }

    // Operators
    public:

        /*
        operator std::string() const { 
            return this->get_string();
        }
        */

    // Internal vatiables
    private:
        long real;
        long imaginary;
        static const long MOD_NUMBER = 580608000 + 10000;
        //static const long MOD_NUMBER =   7;
};
#endif
