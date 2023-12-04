//
// Created by xhl on 6/20/23.
//

#ifndef HOUND_CONSTANTS_HPP
#define HOUND_CONSTANTS_HPP

#include <string>

// todo 可以用 c 风格 str 优化
namespace hd::keys {
	/// kafka 设置
	static const std::string KAFKA_BROKERS{"kafka.brokers"};
	static const std::string KAFKA_TOPICS{"kafka.topic"};
	static const std::string KAFKA_PARTITION{"kafka.partition"};
	static const std::string POOL_INIT_SIZE{"kafka.pool.init-size"};
	static const std::string POOL_MAX_SIZE{"kafka.pool.maxSize"};
	static const std::string CONN_MAX_IDLE_S{"kafka.conn.maxIdleSeconds"};
	static const std::string CONN_TIMEOUT_MS{"kafka.conn.timeoutMilliseconds"};
} // namespace xhl::keys

#endif // HOUND_CONSTANTS_HPP
