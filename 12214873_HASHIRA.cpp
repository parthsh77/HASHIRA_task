
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <sstream>

class ArbitraryInt {
public:
    std::string numberStr;
    bool isNegative;

    ArbitraryInt(long long n = 0) : numberStr(std::to_string(std::abs(n))), isNegative(n < 0) {}
    ArbitraryInt(std::string s) {
        if (s.empty() || s == "0") {
            numberStr = "0";
            isNegative = false;
        } else {
            isNegative = (s[0] == '-');
            numberStr = isNegative ? s.substr(1) : s;
        }
    }

    void trimLeadingZeros() {
        if (numberStr.length() > 1) {
            size_t firstDigit = numberStr.find_first_not_of('0');
            if (std::string::npos != firstDigit) {
                numberStr = numberStr.substr(firstDigit);
            } else {
                numberStr = "0";
                isNegative = false;
            }
        }
        if (numberStr == "0") isNegative = false;
    }

    ArbitraryInt operator+(const ArbitraryInt& other) const;
    ArbitraryInt operator-(const ArbitraryInt& other) const;
    ArbitraryInt operator*(const ArbitraryInt& other) const;
    ArbitraryInt operator/(const ArbitraryInt& other) const;
    bool operator<(const ArbitraryInt& other) const;
    bool operator==(const ArbitraryInt& other) const;

private:
    static std::string addStrings(std::string s1, std::string s2);
    static std::string subtractStrings(std::string s1, std::string s2);
};

std::string ArbitraryInt::addStrings(std::string s1, std::string s2) {
    std::string sum = "";
    int i = s1.length() - 1, j = s2.length() - 1, carry = 0;
    while (i >= 0 || j >= 0 || carry) {
        int d1 = (i >= 0) ? s1[i--] - '0' : 0;
        int d2 = (j >= 0) ? s2[j--] - '0' : 0;
        int current = d1 + d2 + carry;
        sum += std::to_string(current % 10);
        carry = current / 10;
    }
    std::reverse(sum.begin(), sum.end());
    return sum;
}

std::string ArbitraryInt::subtractStrings(std::string s1, std::string s2) {
    std::string diff = "";
    int n1 = s1.length(), n2 = s2.length();
    std::reverse(s1.begin(), s1.end());
    std::reverse(s2.begin(), s2.end());
    int carry = 0;
    for (int i = 0; i < n2; i++) {
        int sub = ((s1[i] - '0') - (s2[i] - '0') - carry);
        if (sub < 0) {
            sub = sub + 10;
            carry = 1;
        } else {
            carry = 0;
        }
        diff.push_back(sub + '0');
    }
    for (int i = n2; i < n1; i++) {
        int sub = ((s1[i] - '0') - carry);
        if (sub < 0) {
            sub = sub + 10;
            carry = 1;
        } else {
            carry = 0;
        }
        diff.push_back(sub + '0');
    }
    std::reverse(diff.begin(), diff.end());
    return diff;
}

ArbitraryInt ArbitraryInt::operator+(const ArbitraryInt& other) const {
    ArbitraryInt result;
    if (isNegative == other.isNegative) {
        result.numberStr = addStrings(numberStr, other.numberStr);
        result.isNegative = isNegative;
    } else {
        ArbitraryInt absThis = *this; absThis.isNegative = false;
        ArbitraryInt absOther = other; absOther.isNegative = false;
        if (absThis < absOther) {
            result.numberStr = subtractStrings(other.numberStr, numberStr);
            result.isNegative = other.isNegative;
        } else {
            result.numberStr = subtractStrings(numberStr, other.numberStr);
            result.isNegative = isNegative;
        }
    }
    result.trimLeadingZeros();
    return result;
}

ArbitraryInt ArbitraryInt::operator-(const ArbitraryInt& other) const {
    ArbitraryInt temp = other;
    if (temp.numberStr != "0") {
        temp.isNegative = !other.isNegative;
    }
    return *this + temp;
}

ArbitraryInt ArbitraryInt::operator*(const ArbitraryInt& other) const {
    if (numberStr == "0" || other.numberStr == "0") return ArbitraryInt(0);
    std::vector<int> res(numberStr.length() + other.numberStr.length(), 0);
    int i_n1 = 0, i_n2 = 0;
    for (int i = numberStr.length() - 1; i >= 0; i--) {
        int carry = 0;
        int n1 = numberStr[i] - '0';
        i_n2 = 0;
        for (int j = other.numberStr.length() - 1; j >= 0; j--) {
            int n2 = other.numberStr[j] - '0';
            int sum = n1 * n2 + res[i_n1 + i_n2] + carry;
            carry = sum / 10;
            res[i_n1 + i_n2] = sum % 10;
            i_n2++;
        }
        if (carry > 0) res[i_n1 + i_n2] += carry;
        i_n1++;
    }
    int i = res.size() - 1;
    while (i >= 0 && res[i] == 0) i--;
    if (i == -1) return ArbitraryInt(0);
    std::string s = "";
    while (i >= 0) s += std::to_string(res[i--]);
    ArbitraryInt result(s);
    result.isNegative = this->isNegative != other.isNegative;
    return result;
}

ArbitraryInt ArbitraryInt::operator/(const ArbitraryInt& other) const {
    if (other.numberStr == "0") throw std::runtime_error("Division by zero");
    ArbitraryInt absDividend = *this; absDividend.isNegative = false;
    ArbitraryInt absDivisor = other; absDivisor.isNegative = false;
    if (absDividend < absDivisor) return ArbitraryInt(0);
    ArbitraryInt quotient(0);
    ArbitraryInt temp(0);
    for (char digit : this->numberStr) {
        temp = temp * ArbitraryInt(10) + ArbitraryInt(digit - '0');
        int count = 0;
        while (!(temp < absDivisor)) {
            temp = temp - absDivisor;
            count++;
        }
        quotient.numberStr += std::to_string(count);
    }
    quotient.trimLeadingZeros();
    quotient.isNegative = (this->isNegative != other.isNegative) && (this->numberStr != "0");
    return quotient;
}

bool ArbitraryInt::operator<(const ArbitraryInt& other) const {
    if (isNegative != other.isNegative) return isNegative;
    if (isNegative) {
        if (numberStr.length() != other.numberStr.length()) return numberStr.length() > other.numberStr.length();
        return numberStr > other.numberStr;
    } else {
        if (numberStr.length() != other.numberStr.length()) return numberStr.length() < other.numberStr.length();
        return numberStr < other.numberStr;
    }
}

bool ArbitraryInt::operator==(const ArbitraryInt& other) const {
    return isNegative == other.isNegative && numberStr == other.numberStr;
}

class Solver {
private:
    struct DataPoint {
        long long x;
        ArbitraryInt y;
    };

    struct Fraction {
        ArbitraryInt numerator, denominator;
    };
    
    int requiredShares = 0;
    std::vector<DataPoint> points;

    static int charToDigit(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    static ArbitraryInt parseValueFromBase(const std::string& valStr, int base) {
        ArbitraryInt result(0);
        ArbitraryInt baseNum(base);
        for (char c : valStr) {
            result = result * baseNum + ArbitraryInt(charToDigit(c));
        }
        return result;
    }

    void parseFile(const std::string& filePath) {
        std::ifstream fileStream(filePath);
        if (!fileStream) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return;
        }

        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        std::string content = buffer.str();
        
        size_t pos = content.find("\"k\":");
        if(pos != std::string::npos) {
            size_t start = content.find_first_of("0123456789", pos);
            size_t end = content.find_first_not_of("0123456789", start);
            requiredShares = std::stoi(content.substr(start, end-start));
        }

        pos = 0;
        while((pos = content.find("\"", pos)) != std::string::npos) {
            size_t startQuote = pos;
            size_t endQuote = content.find("\"", startQuote + 1);
            if (endQuote == std::string::npos) break;

            std::string key = content.substr(startQuote + 1, endQuote - startQuote - 1);
            if(isdigit(key[0])) {
                DataPoint p;
                p.x = std::stoll(key);

                size_t basePos = content.find("\"base\":", endQuote);
                size_t baseStart = content.find("\"", basePos + 7) + 1;
                size_t baseEnd = content.find("\"", baseStart);
                int base = std::stoi(content.substr(baseStart, baseEnd - baseStart));

                size_t valuePos = content.find("\"value\":", baseEnd);
                size_t valueStart = content.find("\"", valuePos + 8) + 1;
                size_t valueEnd = content.find("\"", valueStart);
                p.y = parseValueFromBase(content.substr(valueStart, valueEnd - valueStart), base);
                
                points.push_back(p);
                pos = valueEnd + 1;
            } else {
                pos = endQuote + 1;
            }
        }
    }

public:
    void solveForSecret(const std::string& filePath) {
        parseFile(filePath);
        if (requiredShares == 0 || points.empty()) return;

        Fraction sumOfTerms = {ArbitraryInt(0), ArbitraryInt(1)};

        for (int i = 0; i < requiredShares; ++i) {
            Fraction currentTerm = {points[i].y, ArbitraryInt(1)};

            for (int j = 0; j < requiredShares; ++j) {
                if (i == j) continue;
                
                currentTerm.numerator = currentTerm.numerator * (ArbitraryInt(0) - points[j].x);
                currentTerm.denominator = currentTerm.denominator * (ArbitraryInt(points[i].x) - ArbitraryInt(points[j].x));
            }
            
            sumOfTerms.numerator = (sumOfTerms.numerator * currentTerm.denominator) + (currentTerm.numerator * sumOfTerms.denominator);
            sumOfTerms.denominator = sumOfTerms.denominator * currentTerm.denominator;
        }

        ArbitraryInt secretValue = sumOfTerms.numerator / sumOfTerms.denominator;
        std::cout << secretValue.numberStr << std::endl;
    }
};

int main() {
    try {
        Solver solver1;
        solver1.solveForSecret("testcase1.json");

        Solver solver2;
        solver2.solveForSecret("testcase2.json");
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}