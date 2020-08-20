## 理解Eigen运行机制——以两向量相加为例

考虑以下示例程序：

    #include<Eigen/Core>

    int main() {
        int size = 50;
        Eigen::VectorXf u(size), v(size), w(size);
        u = v + w;
    }

假设SSE2向量化可用。本文档旨在说明Eigen如何执行上述程序。

### 一 引言

对于语句
    
    u = v + w //  (*)

其实现应以以下方式理解，即只进行一次遍历：

    for(int i = 0; i < size; i++) u[i] = v[i] + w[i];

问题在于，若以朴素的做法实现包含VectorXf的C++库并重载返回一个VectorXf类型的运算符+， 则前述语句(*)等价于以下语句：

    VectorXf temp = v + w;
    VectorXf u = temp;

可以看出，以上过程在空间上（temp的多余存储）和时间上（两次遍历）都相对低效的。

利用SIMD优化指令亦是程序编译时需要重点考虑的方面。以SSE2指令集为例，若此指令集可用，则应将向量中的分量以四个一组为单位进行处理，从而减少计算量，提升计算效率。相应语句为

    for(int i = 0; i < 4*(size/4); i+=4) u.packet(i) = v.packet(i) + w.packet(i);
    for(int i = 4*(size/4); i < size; i++) u[i] = v[i] + w[i];

Eigen的执行过程兼顾了以上考虑。

### 二 构造向量

    Eigen::VectorXf u(size), v(size), w(size);

VectorXf对应于以下typedef语句

    typedef Matrix<float, Dynamic, 1> VectorXf;

文件src/Core/util/ForwardDeclarations.h包含了关于模板类Matrix的声明，其中模板参数个数为6，后3个参数由前3个参数确定。Matrix<float, Dynamic, 1>的含义为一个行数不定，列数为1且元素类型为float的矩阵。模板类Matrix继承了统一矩阵/向量与相关表达式的基类MatrixBase。

执行语句

    Eigen::VectorXf u(size);

则会调用文件src/Core/Matrix.h中的构造函数Matrix::Matrix(int)。除执行若干断言外，此构造函数的最关键步骤在于构造类型为DenseStorage<float, Dynamic, Dynamic, 1>的数据成员m_storage。事实上，因存在行/列数不定的情形，矩阵元素会以固定大小数组和指向动态分配数组的指针这两种方式存储。以与模板类Matrix解耦的方式单独实现抽象存储类型，即DenseStorage，是有必要的。

文件src/Core/DenseStorage.h包含了关于模板类DenseStorage的定义, 包括针对在编译时矩阵维数为Dynamic或固定情形的不同模板类偏特化。前述情形对应以下模版类偏特化

    template<typename T, int _Cols> class DenseStorage<T, Dynamic, Dynamic, _Cols>

此偏特化下的构造函数为

    inline DenseStorage(int size, int rows, int) : m_data(internal::aligned_new<T>(size)), m_rows(rows) {}

其中m_data是实际存储矩阵元素的数组。事实上，m_data是动态分配的，且调用的是文件src/Core/util/Memory.h中自定义的internal::aligned_new, 而非标准的new[]或malloc()。当向量化可用，以上自定义函数会分配平台相关的128位对齐数组；若否，则相当于调用标准的new[]。

以上构造函数将DenseStorage中的数据成员m_rows的值设为size（因在向量情形下size==rows）, 且不存在数据成员m_columns。这是因为以上模板类偏特化在产生实例时（即前述DenseStorage<float, Dynamic, Dynamic, 1>）会指定_Cols=1， 因列数为1，因而不需要定义一个存储列数的运行时变量。

当函数VectorXf::data()被调用，其会返回DenseStorage::data(), 即返回数据成员m_data。

当函数VectorXf::size()被调用，此时实际调用的是基类MatrixBase中的一个方法。因标志ColsAtCompileTime == 1（在VectorXf的模板定义中由模板参数衍生），此方法认定涉及向量的类型为列向量，并推断size的大小等于行数，因而其返回VectorXf::rows(), 即调用DenseStorage::rows(), 最终返回m_rows, 而m_rows等于调用构造函数时的参数size。

### 三 构造求和表达式
在Eigen下语句

    u = v + w;
在赋值操作前不执行任何运算，即
    
    v + w
并没有将两个向量相加，而是产生“v与w相加”这一动作的表达式。因VectorXf是模板类Matrix的特化，且模板类MatrixBase是Matrix的基类，v + w实际调用的是MatrixBase中对于运算符+的重载函数
    
    MatrixBase::operator+(const MatrixBase&)
重载函数返回类型是
    
    CwiseBinaryOp<internal::scalar_sum_op<float>, VectorXf, VectorXf>

这是一个表达式模板的实例。CwiseBinaryOp表示逐系数的（Coefficient-wise）的二元（Binary）运算（Operation）,模板实参internal::scalar_sum_op表示对系数作加运算，VectorXf是运算元（operand）的类型。

读者可能会问，对于v + w + u这样语句，因v + w返回类型为CwiseBinaryOp，这是否意味着需要对CwiseBinaryOp重载运算符+？若是如此，以此类推，则每一类表达式中运算符都需要重载。事实上，CwiseBinaryOp这样的表达式模板类被定义为MatrixBase的子类，因而调用的亦是MatrixBase中的重载，从而避免了上述的繁复工作。

运算符的重载集中在基类MatrixBase，而子类没有重载，这似乎无法实现多态性，即不同的子类调用会执行不同的内容。事实上，Eigen应用了C++的模板技巧——奇异复现模板样式（Curiously Recurring Template Pattern (CRTP)）来实现多态性。

    template<typename Derived>
    class CuriousBase {
        ...
    };

    class Curious: public CuriousBase<Curious> {
        ...
    };
子类Curious在继承基类时，基类CuriousBase却以子类Curious本身作为模板实参，这造成了不同的子类继承的实际是子类相关的特定基类。此时我们可以将函数接口集中定义在基类中，而将具体定义放在子类中。例如:

    template<typename Derived>
    class CuriousBase {
        ...
        public：
            Derived& asDerived() {return *static_cast<Derived*>(this);}

            void func() {
                asDerived().funcImpl();
            }
        ...
    };
    
    class Curious: public CuriousBase<Curious> {
        ...
        public:
            void funcImpl() {
                ...
            }
        ...
    };

基类MatrixBase包含了大量方法和运算符重载定义，而CwiseBinaryOp这样的子类中只有少量方法，如coeff()、coeffRef()这些访问矩阵系数的方法，或rows()和cols()这些返回行列数的方法。这正得益于CRTP技巧的应用。不同于应用虚函数的动态多态，应用CRTP技巧获得多态性在编译时即确定了程序的多态行为，避免了虚函数动态多态的运行时开销。

基于以上内容，我们可以更好地理解MatrixBase中运算符+的重载原型

    template<typename Derived>
        class MatrixBase
        {
        // ...
        
        template<typename OtherDerived>
        const CwiseBinaryOp<internal::scalar_sum_op<typename internal::traits<Derived>::Scalar>, Derived, OtherDerived>
        operator+(const MatrixBase<OtherDerived> &other) const;
        
        // ...
        }; 
其中的模板形参Derived和OtherDerived对应类型实参为VectorXf。

CwiseBinaryOp中的第一个模板实参internal::scalar_sum_op是一个函子（functor），即重载了运算符()，从而具有类似函数作用形式的类。文件src/Core/functors/BinaryFunctors.h包含了internal::scalar_sum_op等二元函子的定义。internal::scalar_sum_op也是一个模板，以internal::traits<Derived>作为模板实参，以此特征模板类确定运算涉及的标量类型。以实参替换形参，我们得到以下语句

    class MatrixBase<VectorXf>
    {
    // ...
    
    const CwiseBinaryOp<internal::scalar_sum_op<float>, VectorXf, VectorXf>
    operator+(const MatrixBase<VectorXf> &other) const;
    
    // ...
    };

若进一步检视文件src/Core/CwiseBinary.op的定义，我们可以看出CwiseBinaryOp实际上只是存储了左右运算元的引用和一个函子类的模板实例。概括而言，执行语句v + w产生的只是将运算中各组件暂时存储的临时对象。

### 四 赋值
在v + w产生运算表达式之后，语句u = v + w
中的=导致了Matrix.h中运算符=的重载调用

    template<typename OtherDerived>
    EIGEN_DEVICE_FUNC
    EIGEN_STRONG_INLINE Matrix& operator=(const DenseBase<OtherDerived>& other)
    {
      return Base::_set(other);
    }
因Matrix直接继承模板类PlainObjectBase（即代码中的Base，PlainObjectBase继承MatrixBase，前述基类MatrixBase是Matrix的间接基类)，Base::_set(other)调用的是PlainObjectBase中的成员函数_set：

    template<typename OtherDerived>
    EIGEN_DEVICE_FUNC
    EIGEN_STRONG_INLINE Derived& _set(const DenseBase<OtherDerived>& other)
    {
      internal::call_assignment(this->derived(), other.derived());
      return this->derived();
    }
其中this对应VectorXf u, other对应v + w返回的CwiseBinaryOp

internal::call_assignment对应文件src/Core/AssignEvaluator.h中的call_assignment

    template<typename Dst, typename Src>
    EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
    void call_assignment(Dst& dst, const Src& src)
    {
        call_assignment(dst, src, internal::assign_op<typename Dst::Scalar,typename Src::Scalar>());
    }
AssignEvaluator是Eigen3.3中涉及赋值表达式取值的模块。文件src\Core\functors\AssignmentFunctors中定义的internal::assign_op是封装赋值操作的赋值函子，其会根据模板参数Alignment实例化针对向量化和非向量化的情形的复制操作internal::pstoret。若向量化可用，则Packet对应的是128位对齐的若干数据，例如4个单精度浮点数，pstoret相应是Packet类型的赋值操作。

    template<typename DstScalar,typename SrcScalar> struct assign_op {

        EIGEN_EMPTY_STRUCT_CTOR(assign_op)
        EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void assignCoeff(DstScalar& a, const SrcScalar& b) const { a = b; }
        
        template<int Alignment, typename Packet>
        EIGEN_STRONG_INLINE void assignPacket(DstScalar* a, const Packet& b) const
        { internal::pstoret<DstScalar,Packet,Alignment>(a,b); }
    };

经过若干步跳转，直至调用以下函数模板实例

    template<typename DstXprType, typename SrcXprType, typename Functor>
    EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void call_dense_assignment_loop(DstXprType& dst, const SrcXprType& src, const Functor &func)
    {
        typedef evaluator<DstXprType> DstEvaluatorType;
        typedef evaluator<SrcXprType> SrcEvaluatorType;

        SrcEvaluatorType srcEvaluator(src);

        ...

        DstEvaluatorType dstEvaluator(dst);
            
        typedef generic_dense_assignment_kernel<DstEvaluatorType,SrcEvaluatorType,Functor> Kernel;
        Kernel kernel(dstEvaluator, srcEvaluator, func, dst.const_cast_derived());

        dense_assignment_loop<Kernel>::run(kernel);
    }
其中dst、src和func对应前述call__assignment中的dst、src和assign_op，即u，v+w和赋值函子。srcEvaluator(src)将v + w（CwiseBinaryOp）的运算元的地址和二元运算函子取出并封装为二元运算的evaluator类，相应地dstEvaluator(dst)将u的地址取出并封装为PlainObject（即普通矩阵、向量一类）evaluator类。Kernel类将以上的地址与运算符集中，并执行最终的运算，即调用dense_assignment_loop<Kernel>::run。

        const Index alignedStart = dstIsAligned ? 0 : internal::first_aligned<requestedAlignment>(kernel.dstDataPtr(), size);
        const Index alignedEnd = alignedStart + ((size-alignedStart)/packetSize)*packetSize;

        for(Index index = alignedStart; index < alignedEnd; index += packetSize)
            kernel.template assignPacket<dstAlignment, srcAlignment, PacketType>(index);

        unaligned_dense_assignment_loop<>::run(kernel, alignedEnd, size);

以上语句是dense_assignment_loop<Kernel>::run中最为关键的部分。因SSE可用，所以不管是向量求和或复制运算，都应以Packet为单位，逐单位进行操作。已知每个Packet对应4个float型数据，又知向量的长度（size）为50，因此程序应将每个向量分为前4*12=48个数据和剩余2个数据两部份分别处理，对前48个数据分作12个Packet进行运算，后2个数据每个单独运算。alignedStart（等于0）和alignedEnd（等于48）即进行Packet运算的下标起始与结尾。

语句
    
    kernel.template assignPacket<dstAlignment, srcAlignment, PacketType>(index)
实际调用的是前述函子func（即assign_op）中的assignPacket

    template<int StoreMode, int LoadMode, typename PacketType>
    EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void assignPacket(Index index)
    {
        m_functor.template assignPacket<StoreMode>(&m_dst.coeffRef(index), m_src.template packet<LoadMode,PacketType>(index));
    }

注意assignPacket中的第二个实参m_src.template packet<LoadMode,PacketType>(index)。m_src对应v + w二元运算的evaluator，其中定义了Packet函数调用，即利用从CwiseBinaryOp获取的运算元（地址）和
运算符进行运算。可以看出，evaluator与对应的CwiseBinaryOp都拥有二元运算的运算元与运算符等数据，区别仅在于是否可以实际计算。至此，经过如此繁复的过程，程序才真正开始进行v + w的计算。

    template<int LoadMode, typename PacketType>
    EIGEN_STRONG_INLINE
    PacketType packet(Index index) const
    {
        return m_d.func().packetOp(m_d.lhsImpl.template packet<LoadMode,PacketType>(index),
                                m_d.rhsImpl.template packet<LoadMode,PacketType>(index));
    }

以上代码中的m_d.func对应第三节中的internal::scalar_sum_op，packerOp调用internal::scalar_sum_op对应的实际Packet运算internal::padd。因采用SSE指令集，padd实际调用的是SSE的指令集intrinsic _mm_add_ps。与第一节引言中为变量temp分配空间存储v+w的结果相比，对Packet相加仅需利用一个临时的128位数据存放结果，空间开销大大减小。以上packet函数返回的是两Packet相加结果的地址，assignOp则会利用向量u的地址和此结果地址进行赋值操作。经过若干次循环后，程序完成了对向量u的前48个元素的计算，剩余2个元素经

    unaligned_dense_assignment_loop<>::run(kernel, alignedEnd, size); 
后亦完成了计算。以上便是u = v + w的赋值过程。





    






