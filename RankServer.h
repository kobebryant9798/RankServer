#ifndef RANK_SERVER_H
#define RANK_SERVER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace RankServer {

// 玩家信息结构
struct PlayerInfo {
    int score;                                      // 玩家积分
    time_t timestamp;  // 获得该积分的时间戳
    
    PlayerInfo() : score(0) {}
    PlayerInfo(int s, time_t t) : score(s), timestamp(t) {}
};

// RankServer类：管理一个排名范围的玩家数据
class RankServer {
public:
    /**
     * @brief 构造函数
     */
    RankServer();
    
    /**
     * @brief 析构函数
     */
    ~RankServer();
    
    /**
     * @brief 添加或更新玩家
     * @param playerId 玩家ID
     * @param score 玩家积分
     * @param timestamp 获得该积分的时间戳
     * @return true表示成功，false表示失败
     */
    bool AddPlayer(const std::string& playerId, int score, time_t timestamp);
    
    /**
     * @brief 删除玩家
     * @param playerId 玩家ID
     * @return true表示删除成功，false表示玩家不存在
     */
    bool RemovePlayer(const std::string& playerId);
    
    /**
     * @brief 重新计算所有玩家的排名（相对排名，从1开始）
     */
    void ReRank();
    
    /**
     * @brief 获取当前玩家总数
     * @return 玩家总数
     */
    size_t GetPlayerCount() const;
    
    /**
     * @brief 清空所有玩家数据
     */
    void Clear();

private:
    // 比较函数：用于排序（分数高的在前，分数相同时时间戳小的在前）
    struct ComparePlayer {
        bool operator()(const std::pair<std::string, PlayerInfo>& a, 
                       const std::pair<std::string, PlayerInfo>& b) const {
            if (a.second.score != b.second.score) {
                return a.second.score > b.second.score;  // 分数高的在前
            }
            // 分数相同时，时间戳小的在前（先得到该分数的玩家排在前面）
            return a.second.timestamp < b.second.timestamp;
        }
    };
    
    std::map<std::string, PlayerInfo> m_players;   // 玩家ID到玩家信息的映射
    std::vector<std::pair<std::string, PlayerInfo>> m_rankList;  // 排序后的排名列表
    bool m_rankListDirty;                          // 排名列表是否需要更新
};

} // namespace RankServer

#endif // RANK_SERVER_H

