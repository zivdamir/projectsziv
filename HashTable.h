
#include <cmath>
#include <cassert>
#include <exception>
#include <iostream>
#ifndef HASHTABLE_HASHTABLE_H
#define HASHTABLE_HASHTABLE_H


#define HASH_STEP_CONSTANT 2
#define NOT_FOUND (-1)
#define MINIMUM_ARRAY_SIZE 3
#define MINIMUM_PRIME_ENCODER 3
namespace wet2_dast {
    template<class T>
    class HashTable {

    private:
        class array_object {
        private:
            bool can_insert = true;
            int key;
            T *object;

            friend class HashTable<T>;

        public:
            void setObject(T *object);

            bool CanInsert() const; //returns true if yes
            int getKey() const;

            void change_insertion_status(bool new_status);

            T *getObject() const;

            explicit array_object(T *object = nullptr, int key = 0);

            ~array_object() = default;

            array_object(const array_object &arrayObject);

            array_object &operator=(const array_object &arrayObject);

            bool operator==(const array_object &arrayObject);

        };

        static bool check_if_prime(int number);

        friend class HashTable::array_object;

        int prime_hash_encoder;
        int size;
        array_object *hash_table_array;//will point to t array.
        int current_objects_count = 0;//will count how many objects are in the current array
        void resize(int new_size);

        int Hash_function(int key, int k);//to be worked
        int Hash_function_aux(int key);

        int step_function(int key);

        int find_object_aux(bool to_insert_mode,
                            int key);//two modes,one to_insert_mode==true will find me a nullptr place/can_insert=true.
        //mode 2 (to_insert_mode=false) will find me an actual file
        int closest_prime_to_size(int size);

    public:
        explicit HashTable(int size = MINIMUM_ARRAY_SIZE);



        void insert_object(T *object, int key);

        void delete_object( int key);
        friend void Union (HashTable<T>& destination,HashTable<T>& source){

            for (int i = 0; i < source.size; ++i) {
                //1 by 1
                if (source.hash_table_array[i].getObject() != nullptr &&
                    source.hash_table_array[i].CanInsert() == false) {
                    destination.insert_object(source.hash_table_array[i].getObject(),
                                              source.hash_table_array[i].getKey());
                    source.hash_table_array[i].setObject(nullptr);
                    source.hash_table_array[i].change_insertion_status(true);
                }
            }
            source.resize(MINIMUM_ARRAY_SIZE);
        }

        T *find_object(int key);//returns nullptr 1 if it didn't find anything,else returns the object itself.
        ~HashTable();

        class exceptions : public std::exception {};
    };


    template<class T>
    HashTable<T>::HashTable(int size):size(size) {
        if (size < 0) {
            throw exceptions();
        } else {
            prime_hash_encoder = closest_prime_to_size(size);
            size = prime_hash_encoder;
            this->hash_table_array = new array_object[size];
        }
    }

    template<class T>
    HashTable<T>::~HashTable() {
        for(int i=0; i<size; i++)
        {
            if((hash_table_array + i))

                delete (hash_table_array+i)->getObject();

        }
        delete[] hash_table_array;
    }

    template<class T>
    HashTable<T>::array_object::array_object(T *object, int key):key(key), object(object) {}

    template<class T>
    HashTable<T>::array_object::array_object(const HashTable::array_object &arrayObject) : object(arrayObject.object),
                                                                                           key(arrayObject.key) {}

    template<class T>
    typename HashTable<T>::array_object &
    HashTable<T>::array_object::operator=(const HashTable::array_object &arrayObject) {
        if (*this == arrayObject) {
            return *this;
        }
        this->object = arrayObject.getObject();
        this->key = arrayObject.getKey();
        return *this;
    }

    template<class T>
    int HashTable<T>::array_object::getKey() const {
        return key;
    }


    template<class T>
    T *HashTable<T>::array_object::getObject() const {
        return object;
    }

    template<class T>
    bool HashTable<T>::array_object::operator==(const HashTable::array_object &arrayObject) {
//        if (object == nullptr || arrayObject.getObject() == nullptr) {
//            return (object == arrayObject.getObject()) && (key == arrayObject.getKey());
//        }
//        if (object != nullptr && arrayObject.getObject() != nullptr) {
//            return (key == arrayObject.getKey() && *(this->object) == *(arrayObject.getObject()));
//            //for the question , theoretically I can allocate two objects with the same values in two different memory addresses
////        }
        assert(arrayObject.getKey()>=0&&this->key>=0);
        return key==(arrayObject.getKey());
    }

    template<class T>
    void HashTable<T>::array_object::change_insertion_status(bool new_status) {
        this->can_insert = new_status;
    }

    template<class T>
//return true if it is ok to insert,false otherwise.
    bool HashTable<T>::array_object::CanInsert() const {
        return can_insert;
    }

    template<class T>
    void HashTable<T>::array_object::setObject(T *new_object) {
        this->object = new_object;
    }

    template<class T>
//**changes the size,and copies all objects that were in the old hash table.
//also adjusts prime_hash_enoder
    void HashTable<T>::resize(int new_size) {

        if (new_size <= MINIMUM_ARRAY_SIZE) {
            prime_hash_encoder = MINIMUM_PRIME_ENCODER;
        }
            //first,Allocate new array in new_size.
        else {
            prime_hash_encoder = closest_prime_to_size(new_size); //
        }
        int modified_new_size = prime_hash_encoder;
        auto new_array = new HashTable<T>::array_object[modified_new_size];
        //second,copy all current objects from hash_table_array to new_array;
        current_objects_count = 0;
        array_object *temp = hash_table_array;
        hash_table_array = new_array;
        for (int i = 0; i < size; i++) {
            //if current object, is either nullptr(assume we always start with nullptr when we don't insert anything ) or it can't be inserted(meaning we have value there that we need to move)
            if (temp[i].getObject() != nullptr || (temp[i].CanInsert() == false)) {
                int hash_key = find_object_aux(true, temp[i].getKey());
                hash_table_array[hash_key] = temp[i];
                hash_table_array[hash_key].change_insertion_status(false);
                current_objects_count++;
            }
        }
        delete[] temp;
        size = modified_new_size;

    }

    template<class T>
//actiavtet
//this represents h(x)
    int HashTable<T>::Hash_function_aux(int key) { //
        return key % prime_hash_encoder;
    }

    template<class T>
//this will represent r(x) , the step function.
    int HashTable<T>::step_function(int key) {
        assert(key>=0);
        return 1 + (key % (prime_hash_encoder - HASH_STEP_CONSTANT));
    }

//this will represent h(x)+r(x)
    template<class T>
    int HashTable<T>::Hash_function(int key, int k) { // todo
        assert(k >= 0 && key >= 0);// personally I wouldn't let him go to k, it is just for checking we're okay.
        return (Hash_function_aux(key) + k * step_function(key)) % prime_hash_encoder;
    }

    template<class T>
    void HashTable<T>::insert_object(T *object, int key) { //wip todo
        //if its already exists,then dont insert it again.
        assert(object != nullptr && key >= 0);
        int hashed_key = (find_object_aux(false, key));//query existence
        if (hashed_key != NOT_FOUND) {
            throw exceptions();
        }
        //next , we want to find an index that can be inserted (meaning :nullptr or can be inserted)
        hashed_key = find_object_aux(true, key);//query if we can find place for ihm.
        assert(hashed_key != NOT_FOUND);//this SHOULDN'T HAPPEN,it means our hash-function /find doesnt work properly.
        array_object to_insert(object, key);
        hash_table_array[hashed_key] = to_insert;
        hash_table_array[hashed_key].change_insertion_status(false);
        //this part is wip,we need to do "skipping"
        current_objects_count++;
        if (current_objects_count == size) {
            resize(2 * size);
        }
    }

    template<class T>
    void HashTable<T>::delete_object( int key) { //wip
        //we need to make sure we DO NOT delete nullptr.

        int hashed_key = (find_object_aux(false, key));
        if (hashed_key == NOT_FOUND) {
            return;
        }
        hash_table_array[hashed_key].change_insertion_status(true);
        T *temp = hash_table_array[hashed_key].getObject();
        hash_table_array[hashed_key].setObject(nullptr);
        delete temp;
        current_objects_count--;
        if (current_objects_count <= size / 4) {
            resize(size / 2);
        }

    }
//we need to attain key somehow.

    template<class T>
//for the big QUESTION : ZIV, WHY YOU USE T* OBJECT INSTEAD OF T& or something??
//answer(because I know ill forget it), when I want to find an "empty place", I should look for nullptr.t& cant ge tnullptr
    int HashTable<T>::find_object_aux(bool to_insert_mode, int key) {
        //we should theoritcally have two modes,finding for insertion, finding for existence.
        array_object object_to_find(nullptr, key);
        //we need to find
        int hashed_key;
        //we need to do different cases,when object is not nullptr(strictly for finding), and when he is nullptr(for finding a place to insert him in)
        if (!to_insert_mode) //to_insert_mode=false <==> query existence{
            for (int i = 0; i <= prime_hash_encoder; i++) {
                hashed_key = Hash_function(key, i);
                if ((hash_table_array[hashed_key].getObject() != nullptr) &&
                    hash_table_array[hashed_key] == object_to_find) {
                    return hashed_key;
                }
            }

        if(to_insert_mode)//to insert mode=true <==> query if i can insert it somewhere
        {
            for (int i = 0; i <= prime_hash_encoder; i++) {
                hashed_key = Hash_function(key, i);
                if (hash_table_array[hashed_key] == object_to_find || hash_table_array[hashed_key].CanInsert()) {
                    return hashed_key;
                }
            }
        }
        return NOT_FOUND;
    }

    template<class T>
    int HashTable<T>::closest_prime_to_size(int wanted_size) {
        int prime = wanted_size;
        while (!check_if_prime(prime)) {
            prime++;
        }
        return prime;
    }

    template<class T>
    bool HashTable<T>::check_if_prime(int n) {
        if (n == 1) {
            return false;
        }
        int i = 2;
        while (i * i <= n) {
            if (n % i == 0) {
                return false;
            }
            i += 1;
        }
        return true;
    }

    template<class T>
    T *HashTable<T>::find_object(int key) {
//        assert(key>=0 );//finding nullptr object from outside isn't allowed.nor using negative key.
        int hashed_key = this->find_object_aux(false, key);
        T *to_return;
        if (hashed_key == NOT_FOUND) {
            throw exceptions();
        } else
        {
//             assert(hashed_key>=0);//if this assert fails,it means we've found negative index.
             to_return=hash_table_array[hashed_key].getObject();
        }
        return to_return;
    }

}
#endif //HASHTABLE_HASHTABLE_H
