#pragma once
#ifndef UTILS_OPTIONAL_HEADER
#define UTILS_OPTIONAL_HEADER

#include <exception>
#include <type_traits>

namespace rboc { namespace utils { namespace optional
{
    //! NullOpt
	/*!
	*  NullOpt struct to be able to set a null value.
	*/
    struct NullOpt {

		/*!
		  Literal type to construct NullOpt
		*/ 
        enum class ConstructParam
        {
            Param
        };

		//! NullOpt constructor
        explicit constexpr NullOpt(ConstructParam) {}
    };

    //! NullOpt value.
    static constexpr NullOpt nullopt{NullOpt::ConstructParam::Param};

    //! class BadOptionalException. 
	/*! 
	  Exception to be thrown when an uninitialized Optional is accessed.
	*/
    class BadOptionalException : public std::exception
    {
        public:
        BadOptionalException() 
		{}

        virtual const char* what() const noexcept override
        {
            return "bad optional access";
        }

        virtual ~BadOptionalException() noexcept = default;
    };

    namespace details
    {
        //! OptionalStorage class
		/*! 
		  This is class is designed to manage the storage needed to hold a value of a type
		  that is not trivially destructible.
		*/
        template <typename T, bool = !std::is_trivially_destructible<T>::value>
        struct OptionalStorage
        {
            //! Default constructor
            constexpr OptionalStorage()
            {}

            // Nullopt constructor
            constexpr OptionalStorage(NullOpt)
            {}
			
            //! Destructor            
            ~OptionalStorage()                
            {
                if (_has_value)
                {
                    _value.~T();
                }
            }
            
            //! Copy constructor
            constexpr OptionalStorage(const OptionalStorage& other)
            {
                if (other._has_value)
                {
                    this->construct(other._value);
                }
            }

            //! Move constructor
            constexpr OptionalStorage(OptionalStorage&& other)
            {
                if (other._has_value)
                {
                    this->construct(std::move(other._value));
                    other.reset();
                }
            }

            //! Constructor from optional of convertible type
            template <typename U>
            constexpr OptionalStorage(const OptionalStorage<U>& other)
            {
                if (other._has_value)
                {
                    this->construct(other._value);
                }
            }

            //! Constructor from optional of convertible type
            template <typename U>
                constexpr OptionalStorage(OptionalStorage<U>&& other)                
            {
                if (other._has_value)
                {
                    this->construct(std::move(other._value));
                    other.reset();
                }
            }

            //! Implicit forwarding constructor 
            template <typename U = T, typename = typename std::enable_if<
                std::is_convertible<typename std::decay<U>::type, T>::value>::type>
            constexpr OptionalStorage(U&& other)
                : _value(std::forward<U>(other))
                , _has_value(true)
            {}

            //! Copy assignment
            OptionalStorage& operator=(const OptionalStorage& other)                
            {
                if (other._has_value && this->_has_value)
                {
                    this->_value = other._value;
                }
                else
                {
                    if (other._has_value)
                    {
                        this->construct(other._value);
                    }
                    else
                    {
                        this->reset();
                    }
                }
                return *this;
            }

            //! Move assignment            
            OptionalStorage& operator=(OptionalStorage&& other)                
            {
                if (other._has_value && this->_has_value)
                {
                    this->_value = std::move(other._value);
                }
                else
                {
                    if (other._has_value)
                    {
                        this->construct(std::move(other._value));
                        other.reset();
                    }
                    else
                    {
                        this->reset();
                    }
                }
                return *this;
            }

            //! Nullopt assignment            
            OptionalStorage& operator=(NullOpt)
            {
                this->reset();
                return *this;
            }

            //! Helper to construct values
            template<typename... Args>
            void construct(Args&&... args)
            {
                ::new(std::addressof(_value))T(std::forward<Args>(args)...);
                this->_has_value = true;
            }

            //! Helper to reset values
            void reset()
            {
                if (_has_value)
                {
                    this->_has_value = false;
                    this->_value.~T();
                    this->_empty = Empty{};
                }                
            }

			//! Getter
            T& get()
            {
                assert(_has_value);
                return this->_value;
            }

            // Members
            struct Empty {};
            union
            {
                Empty _empty = {};
                T _value;
            };
            bool _has_value = false;
        };
        
		//! OptionalStorage class
		/*! 
		  This is class is designed to manage the storage needed to hold a value of a type
		  that is trivially destructible.
		*/
        template <typename T>
        struct OptionalStorage<T, false>
        {
            //! Default constructor
            constexpr OptionalStorage()
            {}

			//! NullOpt constructor
            constexpr OptionalStorage(NullOpt)
            {}

            //! Copy constructor
            constexpr OptionalStorage(const OptionalStorage& other)
            {
                if (other._has_value)
                {
                    this->construct(other._value);
                }
            }

            //! Move constructor
            constexpr OptionalStorage(OptionalStorage&& other)  
            {
                if (other._has_value)
                {
                    this->construct(std::move(other)._value);
                    other.reset();
                }
            }

            //! Constructor from optional of convertible type
            template <typename U>
            constexpr OptionalStorage(const OptionalStorage<U>& other)
            {
                if (other._has_value)
                {
                    this->construct(other._value);
                }
            }

            //! Constructor from optional of convertible type
            template <typename U>
            constexpr OptionalStorage(OptionalStorage<U> && other)
            {
                if (other._has_value)
                {
                    this->construct(std::move(other)._value);
                    other.reset();
                }
            }

			//! Implicit forwarding constructor
            template <typename U = T, typename = typename std::enable_if<
                std::is_convertible<typename std::decay<U>::type, T>::value>::type>
                constexpr OptionalStorage(U&& other)
                : _value(std::forward<U>(other))
                , _has_value(true)
            {}

            //! Copy assignment
            OptionalStorage& operator=(const OptionalStorage& other)                
            {
                if (other._has_value && this->_has_value)
                {
                    this->_value = other._value;
                }
                else
                {
                    if (other._has_value)
                    {
                        this->construct(other._value);
                    }
                    else
                    {
                        this->reset();
                    }
                }
                return *this;
            }

            //! Move assignment            
            OptionalStorage& operator=(OptionalStorage&& other)                
            {
                if (other._has_value && this->_has_value)
                {
                    this->_value = std::move(other._value);
                }
                else
                {
                    if (other._has_value)
                    {
                        this->construct(std::move(other._value));
                        other.reset();
                    }
                    else
                    {
                        this->reset();
                    }
                }
                return *this;
            }

            //! Nullopt assignment            
            OptionalStorage& operator=(NullOpt)
            {
                this->reset();
                return *this;
            }

			//! Function to set the value of the storage.
            template<typename... Args>
            void construct(Args&&... args)
            {
                ::new(std::addressof(_value))T(std::forward<Args>(args)...);
                this->_has_value = true;
            }

			//! Function to reset the valuo  of the storage.
            void reset()
            {
                if (_has_value)
                {
                    this->_has_value = false;
                    this->_empty = Empty{};
                }                
            }

			//! Getter
            T& get()
            {
                assert(_has_value);
                return this->_value;
            }

            struct Empty {};
            union
            {
                Empty _empty = {};
                T _value;
            };
            bool _has_value = false;
        };
    }

	//! Optional class.
    /*!
	  This is a class that manages a value that may or may not be contained.
	 */
    template <typename T>
    class Optional
    {
        public:

        using value_type = T;

        //! Default constructor.
        constexpr Optional() = default;        
        
        //! NullOpt constructor.
        constexpr Optional(NullOpt) 
            : _storage(nullopt)
        {}

        //! Destructor.
        ~Optional() = default;

        //! Copy constructor.
        constexpr Optional(const Optional<T>& other)
            : _storage(other._storage)
        {}

        //! Move constructor.
        constexpr Optional(Optional<T>&& other)
            : _storage(std::move(other._storage))
        {}

        //! Conversion from lvalue constructor.
        template <typename U>
        constexpr Optional(const Optional<U>& other)            
        {
            if (other.has_value())
            {
                _storage = other.value();
            }
            else
            {
                _storage.reset();
            }
        }

        //! Conversion from rvalue constructor.
        template <typename U>
        constexpr Optional(Optional<U>&& other)            
        {
            if (other.has_value())
            {
                _storage = std::move(other).value();
                other = nullopt;
            }
            else
            {
                _storage.reset();
            }
        }

        //! Conversion constructor from a forwarding reference of other type U&&.
        template <typename U = T, 
            typename = typename std::enable_if<
            std::is_convertible<typename std::decay<U>::type, T>::value>::type>
        constexpr Optional(U&& value)
            : _storage(std::forward<U>(value))
        {}

        //! Copy assignment.
        Optional<T>& operator=(const Optional<T>& other)
        {
            _storage = other._storage;
            return *this;
        }
        
        //! Move assignment.
        Optional<T>& operator=(Optional<T>&& other)
        {
            _storage = std::move(other._storage);
            return *this;
        };

        //! Nullopt assignment.
        Optional<T>& operator=(NullOpt)
        {
            _storage.reset();
            return *this;
        }
        
        //! Value assignment.
        template<typename U>
        typename std::enable_if<std::is_convertible<typename std::decay<U>::type, T>::value, Optional<T>&>::type
            operator=(U&& u)
        {   
            if (this->_storage._has_value)
            {
                this->_storage._value = std::forward<U>(u);
            }                
            else
            {
                this->_storage.construct(std::forward<U>(u));
            }
            return *this;
        }

        //! lvalue conversion assignment.
        template<typename U>
        typename std::enable_if<std::is_convertible<typename std::decay<U>::type, T>::value, Optional<T>&>::type            
            operator=(const Optional<U>& u)
        {   
            if (u.has_value())
            {
                this->_storage.construct(static_cast<T>(u.value()));
            }
            else
            {
                this->_storage.reset();
            }
            return *this;
        }

        //! rvalue conversion assignment.
        template<typename U>
        typename std::enable_if<std::is_convertible<typename std::decay<U>::type,T>::value, Optional<T>&>::type            
            operator=(Optional<U>&& u)
        {   
            if (u.has_value())
            {
                this->_storage.construct(static_cast<T>(std::move(u).value()));
                u = nullopt;
            }
            else
            {
                this->_storage.reset();
            }

            return *this;
        }
        
        // -------------------------------------------------------------------------------------------------------------
        // Public interface.
        // -------------------------------------------------------------------------------------------------------------

        // Observers.

		//! operator->
        constexpr const T* operator->() const
        { 
            if (this->has_value())
            {
                return std::addressof(this->_storage._value);
            }
            else
            {
                throw BadOptionalException();
            }
        }

		//! operator->
        T* operator->() 
        { 
            if (this->has_value())
            {
                return std::addressof(this->_storage._value);
            }
            else
            {
                throw BadOptionalException();
            }
        }

		//! operator*
        T& operator*(void) &
        {
            if (this->has_value())
            {
                return this->_storage._value;
            }
            else
            {
                throw BadOptionalException();
            }
        }

		//! operator*
        const T& operator*(void) const &
        {
            if (this->has_value())
            {
                return this->_storage._value;
            }
            else
            {
                throw BadOptionalException();
            }
        }

		//! operator*
        T&& operator*() &&
        {
            if (this->has_value())
            {
                return std::move(this->_storage._value);
            }
            else
            {
                throw BadOptionalException();
            }
        }

		//! operator*
        const T&& operator*() const &&
        {
            if (this->has_value())
            {
                return std::move(this->_storage._value);
            }
            else
            {
                throw BadOptionalException();
            }
        }

        //! Operator bool. Checks whether the object contains a value 
        constexpr explicit operator bool() const
        {
            return _storage._has_value;
        }

        //! has_value. Checks whether the object contains a value 
        constexpr bool has_value() const
        {
            return _storage._has_value;
        }

        //! value. Returns the contained value if exists, else throw. 
        constexpr const T& value() const &
        {
            if (_storage._has_value)
            {
                return _storage._value;
            }
            else
            {
                throw BadOptionalException();
            }
        }
        
        //! Overload for rvalue optional references.
        constexpr const T&& value() const &&
        {
            if (_storage._has_value)
            {
                return std::move(_storage._value);
            }
            else
            {
                throw BadOptionalException();
            }
        }

        template< class U >
        constexpr T value_or(U&& default_value) const&
        {
            return _storage._has_value ? _storage._value : static_cast<T>(std::forward<U>(default_value));
        }

        template< class U >
        constexpr T value_or(U&& default_value) &&
        {
            return _storage._has_value ? std::move(_storage._value) : static_cast<T>(std::forward<U>(default_value));
        }
        
        private:

        details::OptionalStorage<T> _storage;
    };

    // -----------------------------------------------------------------------------------------------------------------
    // operators.
    // -----------------------------------------------------------------------------------------------------------------    
    
    //! operator ==
    template <typename T, typename U>
    bool operator == (const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return (lhs && rhs) ? (*lhs == *rhs)  : (!lhs && !rhs);
    }

	//! operator ==
    template <typename T, typename U>
    bool operator == (const T& lhs, const Optional<U>& rhs)
    {
        return rhs && (lhs == *rhs);
    }

	//! operator ==
    template <typename T, typename U>
    bool operator == (const Optional<T>& lhs, const U& rhs)
    {
        return lhs && (*lhs == rhs);
    }

	//! operator ==
    template <typename T>
    bool operator == (const Optional<T>& lhs, NullOpt rhs)
    {
        return !lhs;
    }

	//! operator ==
    template <typename T>
    bool operator == (NullOpt lhs, const Optional<T>& rhs)
    {
        return !rhs;
    }

    //! operator !=
    template <typename T, typename U>
    bool operator != (const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return (lhs && rhs) ? (*lhs != *rhs)  : (lhs || rhs);
    }

	//! operator !=
    template <typename T, typename U>
    bool operator != (const T& lhs, const Optional<U>& rhs)
    {
        return rhs && (lhs != *rhs);
    }

	//! operator !=
    template <typename T, typename U>
    bool operator != (const Optional<T>& lhs, const U& rhs)
    {
        return lhs && (*lhs != rhs);
    }

	//! operator !=
    template <typename T>
    bool operator != (const Optional<T>& lhs, NullOpt rhs)
    {
        return static_cast<bool>(lhs);
    }

	//! operator !=
    template <typename T>
    bool operator != (NullOpt lhs, const Optional<T>& rhs)
    {
        return static_cast<bool>(rhs);
    }

    //! operator <
    template <typename T, typename U>
    bool operator < (const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return rhs && (!lhs || (*lhs < *rhs));
    }

	//! operator <
    template <typename T, typename U>
    bool operator < (const T& lhs, const Optional<U>& rhs)
    {
        return rhs && (lhs < *rhs);
    }

	//! operator <
    template <typename T, typename U>
    bool operator < (const Optional<T>& lhs, const U& rhs)
    {
        return lhs && (*lhs < rhs);
    }

	//! operator <
    template <typename T>
    bool operator < (const Optional<T>& lhs, NullOpt rhs)
    {
        return false;
    }

	//! operator <
    template <typename T>
    bool operator < (NullOpt lhs, const Optional<T>& rhs)
    {
        return static_cast<bool>(rhs);
    }

    //! operator <=
    template <typename T, typename U>
    bool operator <= (const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return rhs && (!lhs || (*lhs <= *rhs));
    }

	//! operator <=
    template <typename T, typename U>
    bool operator <= (const T& lhs, const Optional<U>& rhs)
    {
        return rhs && (lhs <= *rhs);
    }

	//! operator <=
    template <typename T, typename U>
    bool operator <= (const Optional<T>& lhs, const U& rhs)
    {
        return lhs && (*lhs <= rhs);
    }

	//! operator <=
    template <typename T>
    bool operator <= (const Optional<T>& lhs, NullOpt rhs)
    {
        return !lhs;
    }

	//! operator <=
    template <typename T>
    bool operator <= (NullOpt lhs, const Optional<T>& rhs)
    {
        return true;
    }

    //! operator >
    template <typename T, typename U>
    bool operator > (const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return lhs && (!rhs || (*lhs > *rhs));
    }

	//! operator >
    template <typename T, typename U>
    bool operator > (const T& lhs, const Optional<U>& rhs)
    {
        return rhs && (lhs > *rhs);
    }

	//! operator >
    template <typename T, typename U>
    bool operator > (const Optional<T>& lhs, const U& rhs)
    {
        return lhs && (*lhs > rhs);
    }

	//! operator >
    template <typename T>
    bool operator > (const Optional<T>& lhs, NullOpt rhs)
    {
        return lhs.has_value();
    }

	//! operator >
    template <typename T>
    bool operator > (NullOpt lhs, const Optional<T>& rhs)
    {
        return false;
    }

    //! operator >=
    template <typename T, typename U>
    bool operator >= (const Optional<T>& lhs, const Optional<U>& rhs)
    {
        return lhs && (!rhs || (*lhs >= *rhs));
    }

	//! operator >=
    template <typename T, typename U>
    bool operator >= (const T& lhs, const Optional<U>& rhs)
    {
        return rhs && (lhs >= *rhs);
    }

	//! operator >=
    template <typename T, typename U>
    bool operator >= (const Optional<T>& lhs, const U& rhs)
    {
        return lhs && (*lhs >= rhs);
    }

	//! operator >=
    template <typename T>
    bool operator >= (const Optional<T>& lhs, NullOpt rhs)
    {
        return true;
    }

	//! operator >=
    template <typename T>
    bool operator >= (NullOpt lhs, const Optional<T>& rhs)
    {
        return !rhs;
    }

}}} // namespace rboc::utils::optional

#endif
