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

    /**
     * @brief Construct a new Singleton object
     * 
     */
    Singleton() = default;

    /**
     * @brief Destroy the Singleton object
     * 
     */
    virtual ~Singleton() = default;

    public:

    /**
     * @brief Construct a new Singleton object (disabled)
     * 
     */
    Singleton(const Singleton&) = delete;

    /**
     * @brief Construct a new Singleton object (disabled)
     * 
     */
    Singleton(Singleton&&) = delete;

    /**
     * @brief Overloaded = (disabled)
     * 
     * @return Singleton& Reference to singleton object 
     */
    Singleton& operator=(const Singleton&) = delete;

    /**
     * @brief Overloaded = (disabled)
     * 
     * @return Singleton& Reference to singleton object 
     */
    Singleton& operator=(Singleton&&) = delete;

    /**
     * @brief Get the Instance object
     * 
     * @return Singleton& Returns instance of Singleton object. 
     */
    static Singleton& getInstance(){
        static Singleton s;
        return s;
    }

};

};