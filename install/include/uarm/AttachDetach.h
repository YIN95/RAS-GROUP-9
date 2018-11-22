// Generated by gencpp from file uarm/AttachDetach.msg
// DO NOT EDIT!


#ifndef UARM_MESSAGE_ATTACHDETACH_H
#define UARM_MESSAGE_ATTACHDETACH_H

#include <ros/service_traits.h>


#include <uarm/AttachDetachRequest.h>
#include <uarm/AttachDetachResponse.h>


namespace uarm
{

struct AttachDetach
{

typedef AttachDetachRequest Request;
typedef AttachDetachResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct AttachDetach
} // namespace uarm


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::uarm::AttachDetach > {
  static const char* value()
  {
    return "15a30b512eac3d5d3679caf838a84beb";
  }

  static const char* value(const ::uarm::AttachDetach&) { return value(); }
};

template<>
struct DataType< ::uarm::AttachDetach > {
  static const char* value()
  {
    return "uarm/AttachDetach";
  }

  static const char* value(const ::uarm::AttachDetach&) { return value(); }
};


// service_traits::MD5Sum< ::uarm::AttachDetachRequest> should match 
// service_traits::MD5Sum< ::uarm::AttachDetach > 
template<>
struct MD5Sum< ::uarm::AttachDetachRequest>
{
  static const char* value()
  {
    return MD5Sum< ::uarm::AttachDetach >::value();
  }
  static const char* value(const ::uarm::AttachDetachRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::uarm::AttachDetachRequest> should match 
// service_traits::DataType< ::uarm::AttachDetach > 
template<>
struct DataType< ::uarm::AttachDetachRequest>
{
  static const char* value()
  {
    return DataType< ::uarm::AttachDetach >::value();
  }
  static const char* value(const ::uarm::AttachDetachRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::uarm::AttachDetachResponse> should match 
// service_traits::MD5Sum< ::uarm::AttachDetach > 
template<>
struct MD5Sum< ::uarm::AttachDetachResponse>
{
  static const char* value()
  {
    return MD5Sum< ::uarm::AttachDetach >::value();
  }
  static const char* value(const ::uarm::AttachDetachResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::uarm::AttachDetachResponse> should match 
// service_traits::DataType< ::uarm::AttachDetach > 
template<>
struct DataType< ::uarm::AttachDetachResponse>
{
  static const char* value()
  {
    return DataType< ::uarm::AttachDetach >::value();
  }
  static const char* value(const ::uarm::AttachDetachResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // UARM_MESSAGE_ATTACHDETACH_H