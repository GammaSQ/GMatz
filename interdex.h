
#include <stdexcept>
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;

template <class unk>
    class interdex{
private:
    bool copy_;
    int index_num_;
    int value_num_;
    int* indizes_;
    unk* values_;

public:

    interdex()
    {
        copy_=false;
        values_= new unk[0];
        indizes_=new int[0];
    }

    interdex(int value_num, const unk* values)
    {
        copy_=false;
        value_num_=value_num;
        values_ = new unk[value_num];
        index_num_=0;
        indizes_ = new int [0];
        std::copy(values, (values+value_num), values_);
    }

    interdex(int value_num, const unk* values, int index_num, const int* indizes)
    {
        copy_=false;
        value_num_ = value_num;
        index_num_ = index_num;
        values_ = new unk[value_num];
        std::copy(values, (values+value_num), values_);
        indizes_ = new int [index_num];
        std::copy(indizes, (indizes+index_num), indizes_);
    }

    interdex(const interdex<unk>& idx)
    {
        index_num_=idx.index_num_;
        value_num_=idx.value_num_;
        indizes_=new int[idx.index_num_];
        std::copy(idx.indizes_, (idx.indizes_+idx.index_num_), indizes_);
        if(index_num_>0){
            copy_=true;
            values_=idx.values_;
        }else{
            copy_=false;
            values_=new unk [value_num_];
            std::copy(idx.values_, (idx.values_+value_num_), values_);
        }
    }

    interdex(const interdex<unk>& idx, int index_num, int* indizes)
    {
        copy_=true;
        value_num_=idx.value_num_;
        values_=idx.values_;
        index_num_=index_num;
        indizes_=new int [index_num_];
        if(idx.index_num_>0){
            for(int i=0;i<index_num;i++) indizes_[i]=idx.indizes_[indizes[i]];
        }else{
            std::copy(indizes, (indizes+index_num_), indizes_);
        }
    }

    ~interdex()
    {
        if(not (copy_))
        {
            delete[] values_;
        }
        delete[] indizes_;
    }

    int len() const
    {
        int ret=(index_num_==0)? value_num_ : index_num_ ;
        return ret;
    }

    void init(int value_num, const unk* values)
    {
        delete[] values_;
        delete[] indizes_;
        index_num_=0;
        indizes_=new int [0];
        value_num_=value_num;
        values_ = new unk[value_num];
        std::copy(values, (values+value_num), values_);
    }

    void init(interdex<unk> in)
    {
        delete[] values_, indizes_;
        index_num_=0;
        indizes_=new int [0];
        value_num_ = in.len();
        values_ = new unk [value_num_];
        for(int i=0;i<value_num_;i++) values_[i]=in[i];
    }

    void new_indizes(int index_num, int* indizes)
    {
        index_num_=index_num;
        delete [] indizes_;
        int* idx = new int[index_num];
        for(int i=0;i<index_num;i++) idx[i]=indizes_[indizes[i]];
        delete[] indizes_;
        indizes=idx;
    }

    int* selection(int index_num, int* indizes)
    {
        if(index_num_==0) return indizes;
        int* idx=new int [index_num];
        for(int i=0;i<index_num;i++){
            idx[i]=indizes_[indizes[i]];
        }
        return idx;
    }

    int selection(int index) const
    {
        if(index_num_==0)return index;
        return indizes_[index];
    }

    unk& operator[] (const int idx) const
        throw (std::invalid_argument)
    {
        int compair= (index_num_==0)? value_num_ : index_num_;
        if(idx>compair) throw std::invalid_argument("This index is outside of scope!");
        if(index_num_>0){
            return(values_[indizes_[idx]]);
        } else {
            return(values_[idx]);
        }
    }

    int entrys() const
    {
        int ret = (index_num_==0)? value_num_ : index_num_ ;
        return ret;
    }
};
