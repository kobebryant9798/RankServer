#ifndef RANK_CENTER_H
#define RANK_CENTER_H

#include <map>
#include <vector>
#include <string>
#include "Rank.h"

namespace RankServer {

// RankServer排名范围信息
struct RankServerRange {
    int rankServerId;      // RankServer ID
    int minRank;           // 最小排名（包含）
    int maxRank;           // 最大排名（包含）
    
    RankServerRange() : rankServerId(0), minRank(0), maxRank(0) {}
    RankServerRange(int id, int min, int max) : rankServerId(id), minRank(min), maxRank(max) {}
};

// RankCenter类：管理多个RankServer的排行榜数据
class RankCenter {
public:
    /**
     * @brief 构造函数
     */
    RankCenter();
    
    /**
     * @brief 析构函数
     */
    ~RankCenter();
    
    /**
     * @brief 设置或更新RankServer的排名范围
     * @param rankServerId RankServer ID
     * @param minRank 最小排名（包含）
     * @param maxRank 最大排名（包含）
     * @return true表示成功，false表示失败（如排名范围无效）
     */
    bool SetRankRange(int rankServerId, int minRank, int maxRank);
    
    /**
     * @brief 更新或设置某个RankServer的排行榜数据（替换整个vector）
     * @param rankServerId RankServer ID
     * @param rankData 排行榜数据
     */
    void UpdateRankData(int rankServerId, const std::vector<RankInfo>& rankData);
    
    /**
     * @brief 通过玩家id找到玩家的排行榜信息
     * @param playerId 玩家ID
     * @param rankInfo 输出参数：玩家排行榜信息
     * @return true表示找到，false表示未找到
     */
    bool GetPlayerInfo(const std::string& playerId, RankInfo& rankInfo) const;
    
    /**
     * @brief 通过玩家id找到玩家所在的RankServer ID
     * @param playerId 玩家ID
     * @return RankServer ID，如果未找到返回0
     */
    int GetRankServerIdByPlayerId(const std::string& playerId) const;
    
    /**
     * @brief 通过玩家积分找到玩家所在哪个vector里（返回RankServer ID）
     * @param score 玩家积分
     * @return RankServer ID，如果未找到返回0（顺序返回第一个匹配的）
     */
    int GetRankServerIdByScore(int score) const;
    
    /**
     * @brief 获取前N名玩家的分数和名次
     * @param topN 要获取的前N名
     * @return 排名信息列表，按排名从低到高排序（1, 2, 3, ...）
     */
    std::vector<RankInfo> GetTopN(int topN) const;
    
    /**
     * @brief 查询自己名次前后共N个玩家的分数和名次
     * @param playerId 玩家ID
     * @param aroundN 前后各N名（总共2*N+1名，包含自己）
     * @return 排名信息列表，按排名从低到高排序
     */
    std::vector<RankInfo> GetRankAround(const std::string& playerId, int aroundN) const;
    
    /**
     * @brief 获取指定RankServer的排名范围
     * @param rankServerId RankServer ID
     * @param minRank 输出参数：最小排名
     * @param maxRank 输出参数：最大排名
     * @return true表示找到，false表示未找到
     */
    bool GetRankRange(int rankServerId, int& minRank, int& maxRank) const;
    
    /**
     * @brief 移除指定的RankServer
     * @param rankServerId RankServer ID
     * @return true表示成功，false表示未找到
     */
    bool RemoveRankServer(int rankServerId);
    
    /**
     * @brief 获取当前管理的RankServer数量
     * @return RankServer数量
     */
    size_t GetRankServerCount() const;
    
    /**
     * @brief 清空所有RankServer信息
     */
    void Clear();

private:
    // 更新玩家索引（内部方法，在UpdateRankData后调用）
    void UpdatePlayerIndex(int rankServerId);
    
    // 根据排名范围查找RankServer ID（内部方法）
    int FindRankServerByRank(int rank) const;
    
    std::map<int, RankServerRange> m_rankRanges;                    // RankServer ID到排名范围的映射
    std::map<int, std::vector<RankInfo>> m_rankData;                // RankServer ID到排行榜数据的映射
    std::map<std::string, int> m_playerIdToRankServerId;            // 玩家ID到RankServer ID的映射（快速查找）
    std::map<std::string, RankInfo> m_playerIdToRankInfo;           // 玩家ID到RankInfo的映射（快速查找）
    std::vector<RankServerRange> m_sortedRanges;                    // 按排名范围排序的列表（从低到高）
};

} // namespace RankServer

#endif // RANK_CENTER_H
