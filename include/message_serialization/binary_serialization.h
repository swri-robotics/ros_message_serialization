/*
 * Copyright 2018 Southwest Research Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MESSAGE_SERIALIZATION_BINARY_SERIALIZATION_H
#define MESSAGE_SERIALIZATION_BINARY_SERIALIZATION_H

#include <fstream>
#include <ros/serialization.h>
#include <ros/console.h>

namespace message_serialization
{

template<typename T>
inline bool serializeToBinary(const std::string& file,
                              const T& message)
{
  uint32_t serial_size = ros::serialization::serializationLength(message);
  boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);
  ros::serialization::OStream stream(buffer.get(), serial_size);
  ros::serialization::serialize(stream, message);

  std::ofstream ofs(file, std::ios::out|std::ios::binary);
  if(ofs)
  {
    ofs.write((char*) buffer.get(), serial_size);
    return ofs.good();
  }

  return false;
}

template<typename T>
inline bool deserializeFromBinary(const std::string& file,
                                  T& message)
{
  std::ifstream ifs(file, std::ios::in|std::ios::binary);
  if(!ifs)
  {
    return false;
  }

  ifs.seekg(0, std::ios::end);
  std::streampos end = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  std::streampos begin = ifs.tellg();
  uint32_t file_size = end - begin;

  boost::shared_array<uint8_t> ibuffer(new uint8_t[file_size]);
  ifs.read((char*) ibuffer.get(), file_size);

  bool success = true;
  try
  {
    ros::serialization::IStream istream(ibuffer.get(), file_size);
    ros::serialization::deserialize(istream, message);
  }
  catch (const std::exception &ex)
  {
    ROS_ERROR_STREAM("Deserialization error: '" << ex.what() << "'");
    success = false;
  }

  ifs.close();
  return success;
}

/**
 * @brief serializeToBuffer Serialize a ros message into a shared_array<uint8_t>
 * @param buffer out shared_array<uint8_t>
 * @param message in A ros message
 * @return number of bytes in the buffer
 */
template <typename T>
inline uint32_t serializeToBuffer(boost::shared_array<uint8_t>& buffer, const T& message)
{
  uint32_t serial_size = ros::serialization::serializationLength(message);
  buffer.reset(new uint8_t[serial_size]);
  ros::serialization::OStream stream(buffer.get(), serial_size);
  ros::serialization::serialize(stream, message);
  return serial_size;
}

/**
 * @brief deserializeFromBuffer
 * @param buffer in raw uint8_t* Using a raw pointer as a shared_array will force the caller to store their data in one
 * and can lead to undesired delete[]s.  (like sqlite blobs)
 * @param size in size of buffer
 * @param message out a ros message
 * @return success
 */
template <typename T>
inline bool deserializeFromBuffer(const uint8_t* const buffer, const uint32_t size, T& message)
{
  bool success = true;
  try
  {
    ros::serialization::IStream istream(const_cast<uint8_t*>(buffer), size);
    ros::serialization::deserialize(istream, message);
  }
  catch (const std::exception& ex)
  {
    ROS_ERROR_STREAM("Deserialization error: '" << ex.what() << "'");
    success = false;
  }
  return success;
}
}  // namespace message_serialization

#endif // AMSTED_MESSAGE_SERIALIZATION_BINARY_SERIALIZATION_H
