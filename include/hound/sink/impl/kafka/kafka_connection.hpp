//
// Created by xhl on 23-5-11.
//

#ifndef HOUND_KAFKA_CONNECTION_HPP
#define HOUND_KAFKA_CONNECTION_HPP

#include <rdkafkacpp.h>
#include <string>

#include <hound/common/macro.hpp>
#include <hound/sink/impl/kafka/kafka_config.hpp>
#include <hound/sink/impl/kafka/callback/cb_hash_partitioner.hpp>
#include <hound/sink/impl/kafka/callback/cb_producer_delivery_report.hpp>
#include <hound/sink/impl/kafka/callback/cb_producer_event.hpp>

namespace hd::entity {
class kafka_connection {
private:
  clock_t _aliveTime{};
  int m_partition{0};            // 分区
  RdKafka::Conf* m_kafka_conf;   // Kafka Conf 对象
  RdKafka::Conf* m_topic_conf{}; // Topic Conf 对象

  RdKafka::Topic* m_topic{};                  // Topic对象
  RdKafka::Producer* m_connection{};          // Producer对象
  RdKafka::EventCb* m_event_cb{};             // 设置事件回调
  RdKafka::DeliveryReportCb* m_dr_cb{};       // 设置传递回调
  RdKafka::PartitionerCb* m_partitioner_cb{}; // 设置自定义分区回调

public:
  /**
   * @brief message publisher
   */
  explicit kafka_connection(kafka_config::_conn const& conn) {
    std::string error_buffer;
    this->m_partition = conn.partition;
    // 创建配置对象
    this->m_kafka_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    this->m_kafka_conf->set("bootstrap.servers", conn.servers, error_buffer);

    m_dr_cb = new ProducerDeliveryReportCb;
    this->m_kafka_conf->set("dr_cb", m_dr_cb, error_buffer);

    // 设置生产者事件回调
    m_event_cb = new ProducerEventCb;
    this->m_kafka_conf->set("event_cb", m_event_cb, error_buffer);
    this->m_kafka_conf->set("statistics.interval.ms", "10000", error_buffer);
    //  100MB
    this->m_kafka_conf->set("max.message.bytes", "104858800", error_buffer);
    this->m_kafka_conf->set("message.max.bytes", "104858800", error_buffer);

    // 1.2、创建 Topic Conf 对象
    m_topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    m_partitioner_cb = new HashPartitionerCb;
    m_topic_conf->set("partitioner_cb", m_partitioner_cb, error_buffer);

    // 2、创建对象
    this->m_connection =
        RdKafka::Producer::create(this->m_kafka_conf, error_buffer);
    m_topic = RdKafka::Topic::create(m_connection, conn.topics, m_topic_conf,
                                     error_buffer);
  }

  /**
   * @brief push Message to Kafka
   * @param payload, _key
   */
  void pushMessage(const std::string& payload, const std::string& _key) {
    RdKafka::ErrorCode const errorCode = m_connection->produce(
        this->m_topic,
        this->m_partition,
        RdKafka::Producer::RK_MSG_COPY,
        const_cast<char*>(payload.c_str()),
        payload.size(),
        &_key,
        nullptr);

    m_connection->poll(60'000); // timeout ms.
    if (errorCode not_eq RdKafka::ERR_NO_ERROR) {
      hd_line(RED("发送失败: "), err2str(errorCode), CYAN(", 长度: "));
      std::cout << "\033[34;1m" << payload.size() << "\033[0m" << std::endl;
      // kafka 队列满，等待 5000 ms
      if (errorCode == RdKafka::ERR__QUEUE_FULL) {
        m_connection->poll(60'000);
      }
    }
  }

  ~kafka_connection() {
    while (m_connection->outq_len() > 0) {
      hd_line(YELLOW("Connection "),
              std::this_thread::get_id(),
              RED(" Waiting for queue len: "),
              m_connection->outq_len());
      m_connection->flush(1000);
    }
    delete m_kafka_conf;
    delete m_topic_conf;
    delete m_topic;
    delete m_connection;
    delete m_dr_cb;
    delete m_event_cb;
    delete m_partitioner_cb;
  }

  // 刷新连接的起始空闲时刻
  void refreshAliveTime() { _aliveTime = clock(); }

  // 返回连接空闲的时长
  [[nodiscard]] clock_t getAliveTime() const { return clock() - _aliveTime; }
};

} // namespace xhl
#endif // HOUND_KAFKA_CONNECTION_HPP
