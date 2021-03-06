// UnionMetaprogramming.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include <vector>
#include <typeinfo>
#include <iostream>
#include <type_traits>

#if _MSC_VER >= 1900
#define TEMPLATE template<typename> typename
#define TEMPLATE2 template<typename, typename> typename
#define TEMPLATE3 template<typename, typename, typename> typename
#else
#define TEMPLATE template<typename> class
#define TEMPLATE2 template<typename, typename> class
#define TEMPLATE3 template<typename, typename, typename> class
#endif

//template<typename... >
//class TupleView;

template<typename Arg, typename... TArgs>
class Tuple : public Tuple<TArgs...> {

public:
    Tuple() {
        payload = std::move(std::make_unique<Arg>());
    }

    Tuple(const Arg& arg, const TArgs & ... args ) : Tuple<TArgs...>(args ...) {
        payload = std::move(std::make_unique<Arg>());
        *payload = arg;
    }

    template<typename J>
    J& get() {
        return Tuple<TArgs...>::get<J>();
    }

    template<>
    Arg& get() {
        return *payload;
    }

    template<typename J>
    J* tryGet() {
        return Tuple<TArgs...>::tryGet<J>();
    }

    template<>
    Arg* tryGet() {
        return payload.get();
    }

    template<typename Arg2, typename... TArgs2>
    void copyFrom(Tuple<Arg2, TArgs2...>& other) {
        
    }

    template<typename Arg2>
    void tryCopyFrom(Arg2& other) {
        auto p = tryGet<Arg2>();

        if (p) {
            *p = other.get<Arg2>();
        }
    }

    void fill(const Arg& arg, const TArgs & ... args) {
        Tuple(arg, args ...);
    }

private:
    std::unique_ptr<Arg> payload;
};

template<typename Arg>
class Tuple<Arg> {
public:
    template<typename T>
    struct contains : std::false_type {};

    template<>
    struct contains<Arg> : std::true_type {};

public:
    Tuple() {
        payload = std::move(std::make_unique<Arg>());
    }

    Tuple(const Arg& data): Tuple() {
        *payload = data;
    }

    template<typename J>
    J& get() {
        static_assert(false, "do not know what to do ..");
    }

    template<>
    Arg& get() {
        return *payload;
    }

    template<typename J>
    J* tryGet() {
        return nullptr;
    }

    template<>
    Arg* tryGet() {
        return payload.get();
    }

    template<typename Arg2, typename... TArgs2>
    void copyFrom(Tuple<Arg2, TArgs2...>& other) {

    }

    void fill(const Arg& arg) {
        this->Tuple(arg);
    }

private:
    std::unique_ptr<Arg> payload;
};

//template<typename Arg, typename... TArgs>
//class TupleView : public TupleView<TArgs...> {
//public:
//    TupleView(Tuple<Arg, TArgs...>& tuple): tuple(tuple) {
//    }
//
//    Arg& get(Arg* selector = nullptr) {
//        return tuple.get<Arg>();
//    }
//
//private:
//    Tuple<Arg, TArgs...>& tuple;
//};
//
//template<typename Arg>
//class TupleView<Arg> {
//public:
//    TupleView(Tuple<Arg>& tuple) : tuple(tuple) {}
//
//    Arg& get(Arg* selector = nullptr) {
//        return tuple.get<Arg>();
//    }
//
//private:
//    Tuple<Arg>& tuple;
//};

template<typename T, typename TPayload>
class Extension : public T {
public:
    /*template<typename TPayloadOther>
    using Extend = Extension<Extension<T, TPayload>, TPayloadOther>;*/

public:
    Extension() {
        payload = std::move(std::make_unique<TPayload>());
    }

    Extension(const Extension<T, TPayload>& other) : T(other) {
        payload = std::move(std::make_unique<TPayload>());
        *payload = *(other.payload);
    }

    template<typename J>
    J& get() {
        return T::get<J>();
    }

    template<>
    TPayload& get<TPayload>() {
        return *payload;
    }

    template<>
    T& get<T>() {
        return *this;
    }

    template<typename J>
    J* tryGet() {
        return T::tryGet<J>();
    }

    template<>
    TPayload* tryGet<TPayload>() {
        return payload.get();
    }

    template<>
    T* tryGet<T>() {
        return this;
    }

    template<typename J>
    const J& get() const {
        return T::get<J>();
    }

    template<>
    const TPayload& get<TPayload>() const {
        return *payload;
    }

    template<>
    const T& get<T>() const {
        return *this;
    }

    template<typename J>
    const J* tryGet() const {
        return T::tryGet<J>();
    }

    template<>
    const TPayload* tryGet<TPayload>() const {
        return payload.get();
    }

    template<>
    const T* tryGet<T>() const {
        return this;
    }

    Extension& operator=(const Extension& other) {
        static_cast<T&>(*this) = static_cast<const T&>(other);

        payload = std::move(std::make_unique<TPayload>());
        *payload = *(other.payload);

        return *this;
    }
private:
    std::unique_ptr<TPayload> payload;
};


template<typename>
struct Unwrap;

template<typename, typename ...>
struct Join;

template<typename>
struct Union;

template<typename T, typename Arg, typename... TArgs>
struct contains : contains<T, TArgs...> {};

template<typename Arg, typename... TArgs>
struct contains<Arg, Arg, TArgs...> : std::true_type {};

template<typename Arg>
struct contains<Arg, Arg> : std::true_type {};

template<typename Arg, typename T>
struct contains<Arg, T> : std::false_type {};

template<typename T1>
struct Union {

    template<typename... T2Args>
    struct with {
        //using type = Tuple<T1, T2, T2Args... >;
        using type = typename std::conditional<
            contains<T1, T2Args...>::value,  //?
            Tuple<T2Args...>,
            Tuple<T1, T2Args... >>::type;
    };

    template<typename T2>
    struct with<T2> {
        //using type = Tuple<T1, T2>;
        using type = typename std::conditional<
            contains<T1, T2>::value,  //?
            Tuple<T2>,
            Tuple<T1, T2>> ::type;
    };

    using type = Tuple<T1>;
};

template<typename T1>
struct Unwrap {

    template<typename T2>
    struct Combine {
        using type = typename Union<T1>::with<T2>::type; //--> Tuple<T1, T2>
    };

    template<typename T2, typename... T2Args>
    struct Combine<Tuple<T2, T2Args...>> {
        using type = typename Union<T1>::with<T2, T2Args...>::type; //--> Tuple<T1, T2, T2Args ....>
    };

    using type = Tuple<T1>;
};

template<typename T1, typename... T1Args>
struct Unwrap<Tuple<T1, T1Args...>> { //--> Tuple<T1-, T1Args- ...>

    template<typename T2>   //--> T2+
    struct Combine {
        using type = typename Unwrap<T1>::Combine<typename Join<T1Args..., T2>::type>::type;//--> Tuple<T1+, T1Args +..., T2+>
    };

    using type = typename Join<T1, T1Args...>::type;
};


template<typename T1>
struct Unwrap<Tuple<T1>> {  //--> Tuple<T1->

    template<typename T2>   //--> T2+
    struct Combine {
        using type = typename Unwrap<T1>::Combine<T2>::type;//--> Tuple<T1+, T2+>
    };

    using type = typename Join<T1>::type;//--> Tuple<T1+>
};

template<typename T1, typename... TArgs>
struct Join {
    using type = typename Unwrap<T1>::Combine<typename Join<TArgs...>::type>::type;
};

template<typename T1>
struct Join<T1> {
    using type = typename Unwrap<T1>::type;
};



struct Config{};

struct DataFilter2 {};

struct Frame {};

struct Random {};

struct TabulatedDistribution2f {};

struct Vector3{};

/*************************************************************************************
*/

template<typename, typename>
struct DefaultAlgorithm;

template<TEMPLATE2 TAlg, typename ... TParams>
struct AlgorithmTraits {
    using Data = bool;
    using StaticInfo = bool;
    using ModelInfo = bool;
    using LearningInfo = bool;
};

//template<TEMPLATE2 TAlg, typename TModel, typename TMapping>
//struct ContextData {
//    using ImplTraits = AlgorithmTraits<TAlg, TModel, TMapping>;
//
//    template<typename T>
//    using InfoType = typename Join<T>::type;
//
//    using StaticInfo = InfoType<typename ImplTraits::StaticInfo>;
//    using ModelInfo = InfoType<typename ImplTraits::ModelInfo>;
//    using LearningInfo = InfoType<typename ImplTraits::LearningInfo>;
//
//    ContextData(const Config& config): config(config) { }
//public:
//    const Config& config;
//    StaticInfo sinfo;
//    ModelInfo minfo;
//    LearningInfo linfo;
//};
//
//template<TEMPLATE2 TAlg, typename TModel, typename TMapping>
//struct Context {
//    using ImplTraits = AlgorithmTraits<TAlg, TModel, TMapping>;
//
//    template<typename T>
//    using InfoViewType = typename Join<T>::type;
//
//    using StaticInfoView = InfoViewType<typename ImplTraits::StaticInfo>;
//    using ModelInfoView = InfoViewType<typename ImplTraits::ModelInfo>;
//    using LearningInfoView = InfoViewType<typename ImplTraits::LearningInfo>;
//
//    Context(const Config& config) : config(config) {}
//public:
//    const Config& config;
//    StaticInfoView sinfo;
//    ModelInfoView minfo;
//    LearningInfoView linfo;
//};

/*************************************************************************************
Defines concept of learning algorithm for a model. Includes mapping as well because
the data obtained from photon maps and elsewhere need to be transformed in arbitrary
ways suitable for the algorithm itself
*/
template<TEMPLATE2 TImpl, typename TModel, typename TMapping>
struct LearningAlgorithmConcept {
    using Impl = TImpl<TModel, TMapping>;
    using ImplTraits = AlgorithmTraits<TImpl, TModel, TMapping>;
    using Model = TModel;
    using Mapping = TMapping;

    template<typename T>
    using InfoType = typename Join<T>::type;
    /*template<typename T>
    using InfoType = T;*/

    using StaticInfo = InfoType<typename ImplTraits::StaticInfo>;
    using ModelInfo = InfoType<typename ImplTraits::ModelInfo>;
    using LearningInfo = InfoType<typename ImplTraits::LearningInfo>;
    using Data = InfoType<typename ImplTraits::Data>;

    struct Context {
        Context(const Config& config) : config(config) {}

        const Config& config;
        StaticInfo sinfo;
        ModelInfo minfo;
        LearningInfo linfo;
    };

    inline static void init(Context& linfo) {
        static_assert(false, "you have to implement this method in a subclass");
    }

    inline static Data gatherData(Context& context, const DataFilter2& filter, Data prevpass) {
        static_assert(false, "you have to implement this method in a subclass");
    }

    inline static void learn(Context& context, Random& rnd, Model& model, const Data& data) {
        static_assert(false, "you have to implement this method in a subclass");
    }
};

/*************************************************************************************
algorithm decorator
*/
template<typename ... TParams>
struct AlgorithmTraits<DefaultAlgorithm, TParams ...> {
    using Data = Vector3;
    using StaticInfo = bool;
    using ModelInfo = int;
    using LearningInfo = long;
};

template<typename TModel, typename TMapping>
struct DefaultAlgorithm : LearningAlgorithmConcept<DefaultAlgorithm, TModel, TMapping> {
};
//
//template<TEMPLATE2 TImpl, typename TModel, typename TMapping>
//struct AlgorithmTraits<TImpl<TModel, TMapping>>{
//    using StaticInfo = int;
//};


//--------------------------
template<typename TMapping>
struct AlgorithmTraits<DefaultAlgorithm, TabulatedDistribution2f, TMapping> {    
    using Data = std::unique_ptr<std::vector<Vector3>>;
    using StaticInfo = Tuple<std::vector<Vector3>, Frame, Config, int>;
    using ModelInfo = int;
    using LearningInfo = int;
};

template<typename TMapping>
struct DefaultAlgorithm<TabulatedDistribution2f, TMapping> : LearningAlgorithmConcept<DefaultAlgorithm, TabulatedDistribution2f, TMapping> {

    inline static void init(Context& linfo) {}
};


//--------------------------
template<typename TMapping>
struct AlgorithmTraits<DefaultAlgorithm, int, TMapping> {
    using Data = long;
    using StaticInfo = Tuple<std::vector<int>, std::unique_ptr<std::vector<Vector3>>>;
    using ModelInfo = int;
    using LearningInfo = int;
};

template<typename TMapping>
struct DefaultAlgorithm<int, TMapping> : LearningAlgorithmConcept<DefaultAlgorithm, int, TMapping> {

    inline static void init(Context& linfo) {
    }
};


//--------------------------
template<typename TT, typename T, typename TMapping>
struct AlgorithmTraits<DefaultAlgorithm, Extension<TT, T>, TMapping> {
    using InnerAlgorithm = DefaultAlgorithm<TT, TMapping>;
    using Algorithm = DefaultAlgorithm<T, TMapping>;

    using Data = typename InnerAlgorithm::Data;
    using StaticInfo = typename InnerAlgorithm::StaticInfo;
    using ModelInfo = typename InnerAlgorithm::ModelInfo;
    using LearningInfo = typename InnerAlgorithm::LearningInfo;
};

template<typename TT, typename T, typename TMapping>
struct DefaultAlgorithm<Extension<TT, T>, TMapping> : LearningAlgorithmConcept<DefaultAlgorithm, Extension<TT, T>, TMapping> {

    inline static void init(Context& linfo) {
        ImplTraits::InnerAlgorithm::Context *ctx = nullptr;
        ImplTraits::InnerAlgorithm::init(*ctx);
        
        ImplTraits::Algorithm::Context *ctx2 = nullptr;
        ImplTraits::Algorithm::init(*ctx2);
    }
};

/*************************************************************************************
algorithm decorator
*/
template<typename, typename>
struct GatherTrainingDataStatisticsAlgorithm;

struct TrainingDataStatistics {
    std::vector<Vector3> moments;
};


template<typename ... TParams>
struct AlgorithmTraits<GatherTrainingDataStatisticsAlgorithm, TParams ...> {
    using Data = std::unique_ptr<std::vector<Vector3>>;
    using StaticInfo = Tuple<int, TrainingDataStatistics>;
    using ModelInfo = int;
    using LearningInfo = long;
};

template<typename TModel, typename TMapping>
struct GatherTrainingDataStatisticsAlgorithm : LearningAlgorithmConcept<GatherTrainingDataStatisticsAlgorithm, TModel, TMapping> {

    inline static void init(Context& linfo) {
        auto& stat = linfo.sinfo.get<TrainingDataStatistics>();
    }
};

/*************************************************************************************
algorithm decorator
*/
template<typename, typename>
struct PreprocessTrainingDataAlgorithm;

struct PreprocessParams {
    Vector3 scale, bias, clamping;

};

template<typename ... TParams>
struct AlgorithmTraits<PreprocessTrainingDataAlgorithm, TParams ...> {
    using Data = std::unique_ptr<std::vector<Vector3>>;
    using StaticInfo = PreprocessParams;
    using ModelInfo = int;
    using LearningInfo = long;
};

template<typename TModel, typename TMapping>
struct PreprocessTrainingDataAlgorithm : LearningAlgorithmConcept<PreprocessTrainingDataAlgorithm, TModel, TMapping> {

    inline static void init(Context& linfo) {
        linfo.sinfo.get<PreprocessParams>() = PreprocessParams();
    }
};

/*************************************************************************************
algorithm decorator
*/
template<TEMPLATE, TEMPLATE2>
struct Wrap;

template<typename TModel, typename TMapping, TEMPLATE2 TA1, TEMPLATE2 ... TAArgs>
struct AlgorithmCompositionTraits {
    template<typename... TArgs>
    struct GetStaticInfo {
        using type = typename Join<typename TArgs::StaticInfo...>::type;
    };

    template<typename... TArgs>
    struct GetData {
        using type = typename Join<typename TArgs::Data...>::type;
    };


    template<typename... TArgs>
    struct GetModelInfo {
        using type = typename Join<typename TArgs::ModelInfo...>::type;
    };


    template<typename... TArgs>
    struct GetLearningInfo {
        using type = typename Join<typename TArgs::LearningInfo...>::type;
    };

    using Data = typename GetData<TA1<TModel, TMapping>, TAArgs<TModel, TMapping>...>::type;
    using StaticInfo = typename GetStaticInfo<TA1<TModel, TMapping>, TAArgs<TModel, TMapping>...>::type;
    using ModelInfo = typename GetModelInfo<TA1<TModel, TMapping>, TAArgs<TModel, TMapping>...>::type;
    using LearningInfo = typename GetLearningInfo<TA1<TModel, TMapping>, TAArgs<TModel, TMapping>...>::type;
};

template<TEMPLATE2 T1, TEMPLATE2 ...TArgs>
struct Compose {

    template<typename TModel, typename TMapping>
    struct Algorithm {

        using Impl = Algorithm<TModel, TMapping>;
        using ImplTraits = AlgorithmCompositionTraits <TModel, TMapping, T1, TArgs... >;
        using Model = TModel;
        using Mapping = TMapping;


        using StaticInfo = typename ImplTraits::StaticInfo;
        using ModelInfo = typename ImplTraits::ModelInfo;
        using LearningInfo = typename ImplTraits::LearningInfo;
        using Data = typename ImplTraits::Data;

        struct Context {
            Context(const Config& config) : config(config) {}

            const Config& config;
            StaticInfo sinfo;
            ModelInfo minfo;
            LearningInfo linfo;
        };

        inline static void init(Context& linfo) {
            initImpl(linfo, T1<TModel, TMapping>(), TArgs<TModel, TMapping>() ...);
        }

        inline static Data gatherData(Context& context, const DataFilter2& filter, Data prevpass) {
            static_assert(false, "you have to implement this method in a subclass");
        }

        inline static void learn(Context& context, Random& rnd, Model& model, const Data& data) {
            static_assert(false, "you have to implement this method in a subclass");
        }

        
        inline static void initImpl(Context& linfo) {
        }

        template<typename T1, typename... TAlgs>
        inline static void initImpl(Context& linfo, T1 a1, TAlgs... arest) {
            T1::Context ctx(linfo.config);
            linfo.sinfo.copyFrom(ctx.sinfo);
            linfo.minfo.copyFrom(ctx.minfo);
            linfo.linfo.copyFrom(ctx.linfo);

            T1::init(ctx);
            initImpl(linfo, arest...);
        }
    };

    template<TEMPLATE TWrapper>
    using WrapWith = Compose<typename Wrap<TWrapper, T1>::Algorithm, typename Wrap<TWrapper, TArgs>::Algorithm... >;
};

/*************************************************************************************
*/
template<TEMPLATE TWrapper, TEMPLATE2 TAlgorithm>
struct Wrap {

    template<typename TModel, typename TMapping>
    struct Algorithm {

        using Impl = TAlgorithm<TModel, TMapping>;
        using Model = TModel;
        using Mapping = TMapping;

        using StaticInfo = typename Impl::StaticInfo;
        using ModelInfo = typename Impl::ModelInfo;
        using LearningInfo = typename Impl::LearningInfo;
        using Data = typename Impl::Data;
        using Context = typename Impl::Context;

        inline static void init(Context& linfo) {
            TWrapper<Impl>::onInit();
            Impl::init(linfo);
            TWrapper<Impl>::onInit(true);
        }

        inline static Data gatherData(Context& context, const DataFilter2& filter, Data prevpass) {
            
        }

        inline static void learn(Context& context, Random& rnd, Model& model, const Data& data) {
            
        }
    };
};

/*************************************************************************************
algorithm decorator
*/

template<typename TAlgorithm>
struct ReportStage {
    inline static void onInit() {
        std::cout << "initializing " << typeid(TAlgorithm).name();
    }

    inline static void onInit(bool) {
        std::cout << "... done" << std::endl;
    }
};

template<typename TAlgorithm>
struct PersistStage {
    inline static void onInit() {
        std::cout << std::endl;
        std::cout << "\tstoring data for " << typeid(TAlgorithm).name();
    }

    inline static void onInit(bool) {
        std::cout << "... done" << std::endl;
    }
};

int main()
{ 
    using jtype = Join<int, char, char*, Tuple<Random*, int, char*>, Tuple<Tuple<Config, Random, int, char>>>::type;


    using jtype2 = Join<Tuple<Tuple<Random, int, char>>>::type;
    using alg1 =
        DefaultAlgorithm<Extension<TabulatedDistribution2f, int>, int>;
    using alg2 =
        DefaultAlgorithm<TabulatedDistribution2f, int>;

    using alg3 =
        DefaultAlgorithm<int, int>;

    using alg4 =
        Compose< 
            GatherTrainingDataStatisticsAlgorithm, 
            PreprocessTrainingDataAlgorithm,
            GatherTrainingDataStatisticsAlgorithm,
            DefaultAlgorithm
//            ,Compose<
//                GatherTrainingDataStatisticsAlgorithm,
//                PreprocessTrainingDataAlgorithm>
//#ifdef _DEBUG
//            ::WrapWith<ReportStage>
//            ::WrapWith<ReportStage>
//            ::WrapWith<PersistStage>
//#endif        
//            ::Algorithm
        >
#ifdef _DEBUG
        ::WrapWith<ReportStage>
        ::WrapWith<ReportStage>
        ::WrapWith<PersistStage>
#endif
        ::Algorithm<Extension<Extension<TabulatedDistribution2f, int>, int>, int>;

    Config cfg;
    alg1::Context* ddd1 = nullptr;
    alg2::Context* ddd2 = nullptr;
    alg3::Context* ddd3 = nullptr;
    alg4::Context* ddd4 = new alg4::Context(cfg);

    ddd4->linfo.fill(false, false);
    ddd4->sinfo.fill(PreprocessParams(), TrainingDataStatistics(), {}, Frame{}, cfg, 0);
    
    alg1 abc1;
    alg4 abc4;
    //abc4.gatherData()
    alg4::Data aaa;

    //aaa
    //alg2::StaticInfo si;
    alg4::ModelInfo sii;
    alg4::Data data;
    alg4::StaticInfo si;
    alg4::LearningInfo li;
    alg4::ModelInfo mi;

    //ddd4->sinfo.copyTo(si);
    //bbb.get()

    abc4.init(*ddd4);

    jtype xxx;
    int xxx_size = sizeof(jtype);

    return 0;
}

