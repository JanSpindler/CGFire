////
////Created by vincent on 07.07.2021.
////
//
//#pragma once
//
//#include "engine/Spline3D.hpp"
//#include "engine/render/Renderable.hpp"
//
//namespace scene{
//    using namespace en;
//
//    //Connects
//    class ObjToSplineConnector{
//    public:
//        ObjToSplineConnector(std::shared_ptr<Spline3D> spline, std::shared_ptr<RenderObj> obj)
//        : m_Spline(spline),
//        m_Obj(obj),
//        m_isActive(false){
//
//        }
//
//        void Activate(bool onOff = true){
//            m_isActive = onOff;
//        }
//
//        /**time here means local time since Activate() was called
//         * percentage refers to the position on the spline*/
//        void AddKeyPosition(float time, float percentage){
//            m_TimeAndSplinePercent.emplace_back(std::make_pair(time, percentage));
//        }
//
//        void Update(float dt){
//
//        }
//    private:
//        std::shared_ptr<Spline3D> m_Spline;
//        std::shared_ptr<RenderObj> m_Obj;
//        bool m_isActive;
//
//        //We store keypositions here
//        std::vector<std::pair<float, float>> m_TimeAndSplinePercent;
//    };
//}