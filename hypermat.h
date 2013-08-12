
#include "interdex.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

template <class con>
    class hypermat{
public:
    int num_dim_;
    int* dim_;
    interdex<con> interval_;
    char type_;
public:

    hypermat(int num_dim, int *dim):interval_()
    {
        dim_=new int[num_dim];
        std::copy(dim, (dim+num_dim), dim_);
        num_dim_=num_dim;
        int jumpsize=1;
        for(int i=0;i<num_dim_;i++){
            jumpsize*=dim[i];
        }
        con* values=new con [jumpsize];
        for(int i=0; i<jumpsize;i++) values[i]=i+1;
        interval_.init(jumpsize, values);
    }

    hypermat(const hypermat<con>& mat):interval_(mat.interval_)
    {
        num_dim_=mat.num_dim_;
        dim_=new int[num_dim_];
        std::copy(mat.dim_, (mat.dim_+num_dim_), dim_);
    }

    hypermat(int num_dim, int *dim, int jumpsize, con *values):interval_(jumpsize, values)
    {
        dim_=new int[num_dim];
        std::copy(dim, (dim+num_dim), dim_);
        num_dim_= num_dim;
    }

    hypermat(int num_dim, int* dim, interdex<con> indizes):interval_(indizes)
    {
        num_dim_=num_dim;
        dim_=dim;
    }

    ~hypermat()
    {
        delete[] dim_;
    }

    hypermat<con> selection(const hypermat<int> mat)
        throw (std::invalid_argument)
    {
        if (mat.num_dim_>2 or num_dim_ != mat.dim_[0]) throw std::invalid_argument("too many argument dimensions!");
        int* dim=new int [num_dim_];
        int index=0, jumpsize = 1;
        for(int i=0;i<mat.dim_[0];i++){
            for(int j=0;j<mat.dim_[1];j++){
                index=i*mat.dim_[1] + j;
                if(mat.interval_[index]==0){
                    dim[i]=j;
                    break;
                }
            }
            jumpsize *= dim[i];
        }
        int* indizes = new int [jumpsize];
        int dimconst = mat.dim_[1];
        int calc, current, ext;
        for(int i=0;i<jumpsize;i++){
            calc=jumpsize;
            current=i;
            index=0;
            for(int j=0;j<mat.dim_[0];j++){
                calc /= dim[j];
                ext=current/calc;
                index += (mat.interval_[dimconst*j + ext]-1)*calc;
                current -= ext*calc;
            }
            indizes[i]=index;
        }
        int num_dim = mat.dim_[0];
        interdex<con> idx(interval_, jumpsize, indizes);
        hypermat<con>* ret=new hypermat(num_dim, dim, idx);
        return *ret;
    }

    template<class out>
    hypermat<out> map(const int num, const hypermat<con>* mats, out (*func)(con*)) const
        throw(std::invalid_argument)
    {
        int big=num_dim_;
        int jump=interval_.len();
        int biggest=num;
        for(int i=0;i<num;i++)
        {
            bool smaller = big < mats[i].num_dim_;
            big = smaller? mats[i].num_dim_ : big;
            jump = smaller? mats[i].interval_.len() :  jump;
            biggest = smaller? i : biggest;
        }
        int* dims = (biggest==num)? dim_ : mats[biggest].dim_;
        int* dim = new int[big];
        std::copy(dims, (dims+big), dim);
        for(int i=0; i<big; i++){
            bool test=true;
            int testdim=(i<num_dim_)? dim_[i] : 1;
            for(int j=0;j<num;j++){
                if(j>mats[j].num_dim_) continue;
                if(testdim==1) testdim=mats[j].dim_[i];
                if (not (testdim==mats[j].dim_[i] or mats[j].dim_[i]==1)){
                    test=false;
                    break;
                }
            }
            if (not test) throw std::invalid_argument("dimensions don't match!");
            if(((biggest==num)? dim_[i] : mats[biggest].dim_[i])==1) jump *= testdim;
            dim[i]=testdim;
        }
        con *values = new con [jump];
    //////////////////////////////////////////////////
        for(int j=0; j<jump; j++){
            int idx=j;
            int operation=jump;
            int* index = new int[num+1];
            int* exjump = new int[num+1];
            exjump[num]=interval_.len();
            for(int i=0; i<num; i++) exjump[i]=mats[i].interval_.len();

            for(int i=0; i<big;i++){

                operation /= dim[i];

                int basis = ( idx / operation );

                if(i<num_dim_ and dim_[i]!=1){
                    exjump[num] /=dim_[i];
                    int tmp = basis * exjump[num];
                    index[num] += tmp;
                }

                for(int m=0;m<num;m++){
                    if(i<mats[m].num_dim_ and mats[m].dim_[i]!=1){
                        exjump[m] /=mats[m].dim_[i];
                        int tmp = basis * exjump[m];
                        index[m] += tmp;
                    }
                }
                ////// very important! Their can be much more dimensions than jumps, since dimensions can still be 1!
                if(operation==0) break;
                idx = idx%(operation);
            }
            con* eval = new con [num+1];
            eval[0]=interval_[index[num]];
            for(int m=0; m<num;m++){
                eval[m+1]=mats[m].interval_[index[m]];
            }
            values[j]=func(eval);
            delete[] index;
            delete[] exjump;
        }
    //////////////////////////////////////////////////////
        hypermat<con> ret(big, dim, jump, values);
        return ret;
    }

    template <class out, class in>
    hypermat<out> map(const hypermat<in> mat, out (*func)(con,in)) const
        throw(std::invalid_argument)
    {
        if(interval_.len()==mat.interval_.len() and num_dim_==mat.num_dim_){
            hypermat<con> ret(mat);
            for(int i=0;i<interval_.len();i++)ret.interval_[i]=func(interval_[i],mat.interval_[i]);
            return ret;
        }
        bool smaller=num_dim_<mat.num_dim_;
        int small=smaller? num_dim_ : mat.num_dim_;
        int big = smaller? mat.num_dim_ : num_dim_;
        int jump = smaller? mat.interval_.len() : interval_.len();
        int* dim=new int[big];
        if(smaller){
            std::copy(mat.dim_, (mat.dim_+big), dim);
        } else {
            std::copy(dim_, (dim_+big), dim);
        }
        for(int i=0;i<small;i++){
            if(dim_[i]==mat.dim_[i] or dim_[i]==1 or mat.dim_[i]==1){
                if((smaller? mat.dim_[i] : dim_[i])==1){
                    dim[i]=smaller? dim_[i] : mat.dim_[i];
                    jump *= dim[i];
                }
            } else {
                throw std::invalid_argument("dimensions don't match!");
            }
        }
        con *values= new con[jump];


    //////////////////////////////////////
        for(int j=0;j<jump;j++){
            int indexa=0;
            int indexb=0;
            int operation=jump;
            int idx=j;
            int exjumpa=interval_.len();
            int exjumpb=mat.interval_.len();
            for(int i=0;i<big;i++){

                operation /=dim[i];

                if(i<num_dim_ and dim_[i]!=1){
                    exjumpa /=dim_[i];
                    int tmp = ( idx / operation ) * exjumpa;
                    indexa += tmp;
                }
                if(i<mat.num_dim_ and mat.dim_[i]!=1){
                    exjumpb /=mat.dim_[i];
                    int tmp = ( idx / operation ) * exjumpb;
                    indexb +=tmp;
                }
                ////// very important! Their can be much more dimensions than jumps, since dimensions can still be 1!
                if(operation==0) break;
                idx = idx%(operation);
            }
            values[j]=func(interval_[indexa], mat.interval_[indexb]);
        }
    //////////////////////////////////////

        hypermat<out> ret(big, dim, jump, values);
        return ret;
    }

    hypermat<con>& operator = (const hypermat<con>& mat)
        throw ()
    {
        num_dim_=mat.num_dim_;
        dim_=new int[num_dim_];
        std::copy(mat.dim_, (mat.dim_+num_dim_), dim_);
        interval_.init(mat.interval_);
        return *this;
    }

    bool operator == (const hypermat<con> equal) const
        throw (std::invalid_argument)
    {
        if(interval_.len() != equal.interval_.len() or num_dim_ != equal.num_dim_) return false;
        for(int i=0;i<num_dim_;i++){
            if(dim_[i] != equal.dim_[i]) return false;
        }
        for(int i=0;i<interval_.len();i++){
            if(interval_[i] != equal.interval_[i]){
                return false;
            }
        }
        return true;
    }

    hypermat<con> operator + (hypermat<con> mat_to_add) const
        throw (std::invalid_argument)
    {
        if(num_dim_ != mat_to_add.num_dim_ or interval_.len() != mat_to_add.interval_.len())
            throw std::invalid_argument("sizes don't match!");
        for(int i=0;i<num_dim_;i++){
            if(dim_[i] != mat_to_add.dim_[i]){
                throw std::invalid_argument("dimensions don't match!");
            }
        }
        con* values = new con [interval_.len()];
        for(int i=0;i<interval_.len();i++){
            values[i]=interval_[i]+mat_to_add.interval_[i];
        }
        hypermat<con> ret(num_dim_, dim_, interval_.len(), values);
        return ret;
    }

    hypermat<con> operator + (con num_to_add) const
        throw ()
    {
        con* values = new con[interval_.len()];
        for(int i=0;i<interval_.len();i++) values[i]=interval_[i]+num_to_add;
        hypermat<con> ret(num_dim_, dim_, interval_.len(), values);
        return ret;
    }

    virtual hypermat<con>& operator + ()
        throw ()
    {
        return *this;
    }

    hypermat<con> operator - (hypermat<con> mat_to_substract) const
        throw (std::invalid_argument)
    {
        if(num_dim_ != mat_to_substract.num_dim_ or interval_.len() != mat_to_substract.interval_.len())
            throw std::invalid_argument("sizes don't match!");
        for(int i=0;i<num_dim_;i++){
            if(dim_[i] != mat_to_substract.dim_[i]){
                throw std::invalid_argument("dimensions don't match!");
            }
        }
        con* values = new con [interval_.len()];
        for(int i=0;i<interval_.len();i++){
            values[i]=interval_[i]-mat_to_substract.interval_[i];
        }
        hypermat<con> ret(num_dim_, dim_, interval_.len(), values);
        return ret;
    }

    hypermat<con> operator - (con num_to_substract) const
        throw ()
    {
        con* values = new con[interval_.len()];
        for(int i=0;i<interval_.len();i++) values[i]=interval_[i]-num_to_substract;
        hypermat<con> ret(num_dim_, dim_, interval_.len(), values);
        return ret;
    }

    hypermat<con> operator - () const
        throw ()
    {
        con* values=new con [interval_.len()];
        for(int i=0;i<interval_.len();i++) values[i]=-interval_[i];
        return hypermat(num_dim_, dim_, interval_.len(), values);
    }

    //~ virtual hypermat<con>& content(const hypermat<int> mat) const
        //~ throw (std::invalid_argument);

};

//~ template <class con>
//~ hypermat<con>& hypermat<con>::content(const hypermat<int> mat) const
    //~ throw (std::invalid_argument)
//~ {
    //~ if (mat.num_dim_>2 or num_dim_ != mat.dim_[0]) throw std::invalid_argument("too many argument dimensions!");
    //~ int* dim=new int [num_dim_];
    //~ int index, jumpsize = 1;
    //~ for(int i=0;i<mat.dim_[0];i++){
        //~ for(int j=0;j<mat.dim_[1];j++){
            //~ index=i*mat.dim_[1] + j;
            //~ if(mat.interval_[index]==0){
                //~ dim[i]=j;
                //~ break;
            //~ }
        //~ }
        //~ jumpsize *= dim[i];
    //~ }
    //~ con* values = new con [jumpsize];
    //~ int dimconst = mat.dim_[1];
    //~ int calc, current, ext;
    //~ for(int i=0;i<jumpsize;i++){
        //~ calc=jumpsize;
        //~ current=i;
        //~ index=0;
        //~ for(int j=0;j<mat.dim_[0];j++){
            //~ calc /= dim[j];
            //~ ext=current/calc;
            //~ index += (mat.interval_[dimconst*j + ext]-1)*calc;
            //~ current -= ext*calc;
        //~ }
        //~ values[i]=interval_[index];
    //~ }
    //~ int num_dim = mat.dim_[0];
    //~ hypermat<con>* ret=new hypermat(num_dim, dim, jumpsize, values);
    //~ return *ret;
//~ }
