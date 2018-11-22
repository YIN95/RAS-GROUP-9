// Generated by gencpp from file ras_lab1_msgs/ControllerParams.msg
// DO NOT EDIT!


#ifndef RAS_LAB1_MSGS_MESSAGE_CONTROLLERPARAMS_H
#define RAS_LAB1_MSGS_MESSAGE_CONTROLLERPARAMS_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace ras_lab1_msgs
{
template <class ContainerAllocator>
struct ControllerParams_
{
  typedef ControllerParams_<ContainerAllocator> Type;

  ControllerParams_()
    : K(0)
    , KI(0)
    , INT_MAX(0)
    , ticks(0)
    , r(0.0)
    , r_l(0.0)
    , r_r(0.0)
    , B(0.0)  {
    }
  ControllerParams_(const ContainerAllocator& _alloc)
    : K(0)
    , KI(0)
    , INT_MAX(0)
    , ticks(0)
    , r(0.0)
    , r_l(0.0)
    , r_r(0.0)
    , B(0.0)  {
  (void)_alloc;
    }



   typedef uint16_t _K_type;
  _K_type K;

   typedef uint16_t _KI_type;
  _KI_type KI;

   typedef uint16_t _INT_MAX_type;
  _INT_MAX_type INT_MAX;

   typedef uint16_t _ticks_type;
  _ticks_type ticks;

   typedef float _r_type;
  _r_type r;

   typedef float _r_l_type;
  _r_l_type r_l;

   typedef float _r_r_type;
  _r_r_type r_r;

   typedef float _B_type;
  _B_type B;





  typedef boost::shared_ptr< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> const> ConstPtr;

}; // struct ControllerParams_

typedef ::ras_lab1_msgs::ControllerParams_<std::allocator<void> > ControllerParams;

typedef boost::shared_ptr< ::ras_lab1_msgs::ControllerParams > ControllerParamsPtr;
typedef boost::shared_ptr< ::ras_lab1_msgs::ControllerParams const> ControllerParamsConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace ras_lab1_msgs

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': True, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['/opt/ros/kinetic/share/std_msgs/cmake/../msg'], 'ras_lab1_msgs': ['/home/ras29/catkin_ws/src/ras_lab1/ras_lab1_msgs/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
{
  static const char* value()
  {
    return "8950f824993d2867219a00186d4620f7";
  }

  static const char* value(const ::ras_lab1_msgs::ControllerParams_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x8950f824993d2867ULL;
  static const uint64_t static_value2 = 0x219a00186d4620f7ULL;
};

template<class ContainerAllocator>
struct DataType< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
{
  static const char* value()
  {
    return "ras_lab1_msgs/ControllerParams";
  }

  static const char* value(const ::ras_lab1_msgs::ControllerParams_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
{
  static const char* value()
  {
    return "# PI Control parameters\n\
uint16 K\n\
uint16 KI\n\
uint16 INT_MAX\n\
uint16 ticks\n\
\n\
# Robot dimensions\n\
float32 r\n\
float32 r_l\n\
float32 r_r\n\
float32 B\n\
";
  }

  static const char* value(const ::ras_lab1_msgs::ControllerParams_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.K);
      stream.next(m.KI);
      stream.next(m.INT_MAX);
      stream.next(m.ticks);
      stream.next(m.r);
      stream.next(m.r_l);
      stream.next(m.r_r);
      stream.next(m.B);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct ControllerParams_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::ras_lab1_msgs::ControllerParams_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::ras_lab1_msgs::ControllerParams_<ContainerAllocator>& v)
  {
    s << indent << "K: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.K);
    s << indent << "KI: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.KI);
    s << indent << "INT_MAX: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.INT_MAX);
    s << indent << "ticks: ";
    Printer<uint16_t>::stream(s, indent + "  ", v.ticks);
    s << indent << "r: ";
    Printer<float>::stream(s, indent + "  ", v.r);
    s << indent << "r_l: ";
    Printer<float>::stream(s, indent + "  ", v.r_l);
    s << indent << "r_r: ";
    Printer<float>::stream(s, indent + "  ", v.r_r);
    s << indent << "B: ";
    Printer<float>::stream(s, indent + "  ", v.B);
  }
};

} // namespace message_operations
} // namespace ros

#endif // RAS_LAB1_MSGS_MESSAGE_CONTROLLERPARAMS_H