// Generated by gencpp from file uarm/Pump.msg
// DO NOT EDIT!


#ifndef UARM_MESSAGE_PUMP_H
#define UARM_MESSAGE_PUMP_H

#include <ros/service_traits.h>


#include <uarm/PumpRequest.h>
#include <uarm/PumpResponse.h>


namespace uarm
{

struct Pump
{

typedef PumpRequest Request;
typedef PumpResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct Pump
} // namespace uarm


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::uarm::Pump > {
  static const char* value()
  {
    return "ff5a115a491fb65f575572e4a97ae33c";
  }

  static const char* value(const ::uarm::Pump&) { return value(); }
};

template<>
struct DataType< ::uarm::Pump > {
  static const char* value()
  {
    return "uarm/Pump";
  }

  static const char* value(const ::uarm::Pump&) { return value(); }
};


// service_traits::MD5Sum< ::uarm::PumpRequest> should match 
// service_traits::MD5Sum< ::uarm::Pump > 
template<>
struct MD5Sum< ::uarm::PumpRequest>
{
  static const char* value()
  {
    return MD5Sum< ::uarm::Pump >::value();
  }
  static const char* value(const ::uarm::PumpRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::uarm::PumpRequest> should match 
// service_traits::DataType< ::uarm::Pump > 
template<>
struct DataType< ::uarm::PumpRequest>
{
  static const char* value()
  {
    return DataType< ::uarm::Pump >::value();
  }
  static const char* value(const ::uarm::PumpRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::uarm::PumpResponse> should match 
// service_traits::MD5Sum< ::uarm::Pump > 
template<>
struct MD5Sum< ::uarm::PumpResponse>
{
  static const char* value()
  {
    return MD5Sum< ::uarm::Pump >::value();
  }
  static const char* value(const ::uarm::PumpResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::uarm::PumpResponse> should match 
// service_traits::DataType< ::uarm::Pump > 
template<>
struct DataType< ::uarm::PumpResponse>
{
  static const char* value()
  {
    return DataType< ::uarm::Pump >::value();
  }
  static const char* value(const ::uarm::PumpResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // UARM_MESSAGE_PUMP_H
