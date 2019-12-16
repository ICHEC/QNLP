/**
 * @file Singleton.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Singleton object pattern header. Follows Meyers singleton pattern
 * @version 0.1
 * @date 2019-11-15
 */

namespace QNLP{

/**
 * @brief Follows the Meyers singleton pattern, allowing thread-safe access to singleton object
 * 
 */
class Singleton{
    private:

    Singleton() = default;
    virtual ~Singleton() = default;

    public:

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;

    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static Singleton& getInstance(){
        static Singleton s;
        return s;
    }

};

};