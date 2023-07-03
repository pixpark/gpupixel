/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */
#ifndef MACROS_DEFINE_H_
#define MACROS_DEFINE_H_

#define SINGLETON_DECLARE_LOG(className) \
 public:                                 \
  static className* Instance() {         \
    static className m_instance;         \
    return &m_instance;                  \
  }                                      \
                                         \
 private:                                \
  className(const className&) = delete;  \
  className& operator=(const className&) = delete;

#endif
