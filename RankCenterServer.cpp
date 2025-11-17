#include "RankCenter.h"
#include <algorithm>

namespace RankServer {

RankCenter::RankCenter() {
}

RankCenter::~RankCenter() {
}

bool RankCenter::SetRankRange(int rankServerId, int minRank, int maxRank) {
    // 检查排名范围是否有效
    if (minRank < 1 || maxRank < minRank) {
        return false;  // 排名范围无效
    }
    
    // 设置或更新RankServer的排名范围
    m_rankRanges[rankServerId] = RankServerRange(rankServerId, minRank, maxRank);
    
    // 更新排序列表
    m_sortedRanges.clear();
    m_sortedRanges.reserve(m_rankRanges.size());
    for (const auto& pair : m_rankRanges) {
        m_sortedRanges.push_back(pair.second);
    }
    // 按照最小排名从低到高排序
    std::sort(m_sortedRanges.begin(), m_sortedRanges.end(), 
        [](const RankServerRange& a, const RankServerRange& b) {
            return a.minRank < b.minRank;
        });
    
    return true;
}

void RankCenter::UpdateRankData(int rankServerId, const std::vector<RankInfo>& rankData) {
    // 更新排行榜数据
    m_rankData[rankServerId] = rankData;
    
    // 更新玩家索引
    UpdatePlayerIndex(rankServerId);
}

bool RankCenter::GetPlayerInfo(const std::string& playerId, RankInfo& rankInfo) const {
    auto it = m_playerIdToRankInfo.find(playerId);
    if (it != m_playerIdToRankInfo.end()) {
        rankInfo = it->second;
        return true;
    }
    return false;
}

int RankCenter::GetRankServerIdByPlayerId(const std::string& playerId) const {
    auto it = m_playerIdToRankServerId.find(playerId);
    if (it != m_playerIdToRankServerId.end()) {
        return it->second;
    }
    return 0;
}

int RankCenter::GetRankServerIdByScore(int score) const {
    // 按照排名范围顺序遍历所有RankServer，查找包含该分数的RankServer
    // 顺序返回第一个匹配的
    for (const auto& range : m_sortedRanges) {
        int rankServerId = range.rankServerId;
        auto it = m_rankData.find(rankServerId);
        if (it == m_rankData.end()) {
            continue;  // 该RankServer没有数据
        }
        
        const auto& rankData = it->second;
        
        // 在排行榜数据中查找是否有玩家分数等于该分数
        for (const auto& rankInfo : rankData) {
            if (rankInfo.score == score) {
                return rankServerId;
            }
        }
    }
    
    return 0;
}

std::vector<RankInfo> RankCenter::GetTopN(int topN) const {
    std::vector<RankInfo> result;
    
    if (topN <= 0) {
        return result;
    }
    
    // 按照排名范围从低到高遍历（vector1存储1-10000，vector2存储10001-20000等）
    for (const auto& range : m_sortedRanges) {
        int rankServerId = range.rankServerId;
        auto it = m_rankData.find(rankServerId);
        if (it == m_rankData.end()) {
            continue;  // 该RankServer没有数据
        }
        
        const auto& rankData = it->second;
        
        // 从当前RankServer的排行榜数据中取数据
        for (const auto& rankInfo : rankData) {
            if (static_cast<int>(result.size()) >= topN) {
                break;  // 已经取够N名
            }
            result.push_back(rankInfo);
        }
        
        if (static_cast<int>(result.size()) >= topN) {
            break;  // 已经取够N名
        }
    }
    
    return result;
}

std::vector<RankInfo> RankCenter::GetRankAround(const std::string& playerId, int aroundN) const {
    std::vector<RankInfo> result;
    
    if (aroundN < 0) {
        return result;
    }
    
    // 先找到玩家信息
    auto playerIt = m_playerIdToRankInfo.find(playerId);
    if (playerIt == m_playerIdToRankInfo.end()) {
        return result;  // 玩家不存在
    }
    
    const RankInfo& playerInfo = playerIt->second;
    int playerRank = playerInfo.rank;
    int minRank = (playerRank - aroundN > 1) ? (playerRank - aroundN) : 1;
    int maxRank = playerRank + aroundN;
    
    // 收集所有需要返回的排名信息
    // 需要遍历所有RankServer，因为可能跨vector
    std::map<int, RankInfo> rankMap;  // 使用map按排名排序
    
    for (const auto& pair : m_rankData) {
        const auto& rankData = pair.second;
        
        for (const auto& rankInfo : rankData) {
            if (rankInfo.rank >= minRank && rankInfo.rank <= maxRank) {
                rankMap[rankInfo.rank] = rankInfo;
            }
        }
    }
    
    // 转换为vector返回
    result.reserve(rankMap.size());
    for (const auto& pair : rankMap) {
        result.push_back(pair.second);
    }
    
    return result;
}

bool RankCenter::GetRankRange(int rankServerId, int& minRank, int& maxRank) const {
    auto it = m_rankRanges.find(rankServerId);
    if (it != m_rankRanges.end()) {
        minRank = it->second.minRank;
        maxRank = it->second.maxRank;
        return true;
    }
    return false;
}

bool RankCenter::RemoveRankServer(int rankServerId) {
    bool found = false;
    
    // 移除排名范围
    auto rangeIt = m_rankRanges.find(rankServerId);
    if (rangeIt != m_rankRanges.end()) {
        m_rankRanges.erase(rangeIt);
        found = true;
    }
    
    // 移除排行榜数据
    auto dataIt = m_rankData.find(rankServerId);
    if (dataIt != m_rankData.end()) {
        m_rankData.erase(dataIt);
        found = true;
    }
    
    // 更新玩家索引（移除该RankServer的所有玩家）
    for (auto it = m_playerIdToRankServerId.begin(); it != m_playerIdToRankServerId.end();) {
        if (it->second == rankServerId) {
            m_playerIdToRankInfo.erase(it->first);
            it = m_playerIdToRankServerId.erase(it);
        } else {
            ++it;
        }
    }
    
    // 更新排序列表
    if (found) {
        m_sortedRanges.clear();
        m_sortedRanges.reserve(m_rankRanges.size());
        for (const auto& pair : m_rankRanges) {
            m_sortedRanges.push_back(pair.second);
        }
        std::sort(m_sortedRanges.begin(), m_sortedRanges.end(), 
            [](const RankServerRange& a, const RankServerRange& b) {
                return a.minRank < b.minRank;
            });
    }
    
    return found;
}

size_t RankCenter::GetRankServerCount() const {
    return m_rankRanges.size();
}

void RankCenter::Clear() {
    m_rankRanges.clear();
    m_rankData.clear();
    m_playerIdToRankServerId.clear();
    m_playerIdToRankInfo.clear();
    m_sortedRanges.clear();
}

void RankCenter::UpdatePlayerIndex(int rankServerId) {
    // 先移除该RankServer的旧索引
    for (auto it = m_playerIdToRankServerId.begin(); it != m_playerIdToRankServerId.end();) {
        if (it->second == rankServerId) {
            m_playerIdToRankInfo.erase(it->first);
            it = m_playerIdToRankServerId.erase(it);
        } else {
            ++it;
        }
    }
    
    // 添加新的索引
    auto it = m_rankData.find(rankServerId);
    if (it != m_rankData.end()) {
        const auto& rankData = it->second;
        for (const auto& rankInfo : rankData) {
            m_playerIdToRankServerId[rankInfo.playerId] = rankServerId;
            m_playerIdToRankInfo[rankInfo.playerId] = rankInfo;
        }
    }
}

int RankCenter::FindRankServerByRank(int rank) const {
    // 根据排名查找对应的RankServer
    for (const auto& range : m_sortedRanges) {
        if (rank >= range.minRank && rank <= range.maxRank) {
            return range.rankServerId;
        }
    }
    return 0;
}

} // namespace RankServer
