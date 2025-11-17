#include "RankServer.h"
#include <algorithm>

namespace RankServer {

RankServer::RankServer() : m_rankListDirty(true) {
}

RankServer::~RankServer() {
}

bool RankServer::AddPlayer(const std::string& playerId, int score, time_t timestamp) {
    // 更新或插入玩家信息
    auto it = m_players.find(playerId);
    if (it != m_players.end()) {
        // 玩家已存在，更新分数和时间戳
        it->second.score = score;
        it->second.timestamp = timestamp;
    } else {
        // 新玩家
        m_players[playerId] = PlayerInfo(score, timestamp);
    }
    
    // 标记排名列表需要更新
    m_rankListDirty = true;
    
    return true;
}

bool RankServer::RemovePlayer(const std::string& playerId) {
    auto it = m_players.find(playerId);
    if (it != m_players.end()) {
        m_players.erase(it);
        // 标记排名列表需要更新
        m_rankListDirty = true;
        return true;
    }
    
    return false;
}

void RankServer::ReRank() {
    // 清空当前排名列表
    m_rankList.clear();
    m_rankList.reserve(m_players.size());
    
    // 将所有玩家信息复制到排名列表
    for (const auto& pair : m_players) {
        m_rankList.push_back(pair);
    }
    
    // 按照排名规则排序
    std::sort(m_rankList.begin(), m_rankList.end(), ComparePlayer());
    
    // 标记排名列表已更新
    m_rankListDirty = false;
}

size_t RankServer::GetPlayerCount() const {
    return m_players.size();
}

void RankServer::Clear() {
    m_players.clear();
    m_rankList.clear();
    m_rankListDirty = true;
}

} // namespace RankServer

