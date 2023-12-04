//
// Created by xhl on 23-5-11.
//

#ifndef HOUND_PRODUCER_EVENT_CB_HPP
#define HOUND_PRODUCER_EVENT_CB_HPP

#include <iostream>
#include <rdkafkacpp.h>

// 生产者事件回调函数
class ProducerEventCb : public RdKafka::EventCb {
public:
  void event_cb(RdKafka::Event& event) override {
    switch (event.type()) {
      case RdKafka::Event::EVENT_ERROR:
        // hd_info(RED("EVENT_ERROR: "), event.str());
        break;
      case RdKafka::Event::EVENT_STATS:
        // hd_debug(BLUE("EVENT_STATS: "), event.str());
        break;
      case RdKafka::Event::EVENT_LOG:
        // hd_debug(BLUE("EVENT_LOG: "), event.fac());
        break;
      case RdKafka::Event::EVENT_THROTTLE:
        // hd_debug(RED("EVENT_THROTTLE: "), event.broker_name());
        break;
    }
  }

private:
};

#endif // HOUND_PRODUCER_EVENT_CB_HPP