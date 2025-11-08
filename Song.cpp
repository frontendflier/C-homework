#include "Song.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

namespace {

// 去除字符串首尾空白
std::string trim_copy(const std::string& s) {
    if (s.empty()) {
        return "";
    }
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

// 转换字符串为小写
std::string to_lower_copy(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return res;
}

// 连接标签为逗号分隔的字符串
std::string join_tags(const std::vector<std::string>& tags) {
    std::stringstream ss;
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << tags[i];
    }
    return ss.str();
}

}  // 匿名命名空间结束

// 初始化静态成员变量
int Song::next_id_ = 1;

// 构造函数实现
Song::Song(const std::string& title, const std::string& artist,
           int duration_sec, int rating) {
    std::string trimmed_title = trim_copy(title);
    std::string trimmed_artist = trim_copy(artist);

    bool has_error = false;
    if (trimmed_title.empty()) {
        std::cerr << "[错误] 标题不能为空\n";
        has_error = true;
    }
    if (trimmed_artist.empty()) {
        std::cerr << "[错误] 艺人不能为空\n";
        has_error = true;
    }
    if (duration_sec <= 0) {
        std::cerr << "[错误] 时长必须为正整数（秒）\n";
        has_error = true;
    }
    if (rating < 1 || rating > 5) {
        std::cerr << "[错误] 评分必须在 1...5 之间\n";
        has_error = true;
    }

    if (has_error) {
        valid_ = false;
        return;
    }

    id_ = next_id_++;
    title_ = trimmed_title;
    artist_ = trimmed_artist;
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true;
}

// Setter函数实现
bool Song::set_title(const std::string& t) {
    std::string trimmed = trim_copy(t);
    if (trimmed.empty()) {
        std::cerr << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = trimmed;
    return true;
}

bool Song::set_artist(const std::string& a) {
    std::string trimmed = trim_copy(a);
    if (trimmed.empty()) {
        std::cerr << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = trimmed;
    return true;
}

bool Song::set_duration(int sec) {
    if (sec <= 0) {
        std::cerr << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        std::cerr << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}

// 标签管理函数实现
bool Song::add_tag(const std::string& tag) {
    std::string trimmed_tag = trim_copy(tag);
    if (trimmed_tag.empty()) {
        std::cerr << "[提示] 空标签忽略。\n";  // 匹配预期的提示信息格式
        return false;
    }

    std::string lower_tag = to_lower_copy(trimmed_tag);
    for (const auto& existing : tags_) {
        if (to_lower_copy(existing) == lower_tag) {
            std::cerr << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }

    tags_.push_back(trimmed_tag);
    return true;
}

bool Song::remove_tag(const std::string& tag) {
    std::string target = to_lower_copy(trim_copy(tag));
    for (size_t i = 0; i < tags_.size(); ++i) {
        if (to_lower_copy(tags_[i]) == target) {
            tags_.erase(tags_.begin() + i);
            return true;
        }
    }

    std::cerr << "[提示] 未找到该标签\n";
    return false;
}

// 关键词匹配函数实现
bool Song::matches_keyword(const std::string& kw) const {
    std::string trimmed_kw = trim_copy(kw);
    if (trimmed_kw.empty()) {
        return false;
    }

    std::string lower_kw = to_lower_copy(trimmed_kw);

    if (to_lower_copy(title_).find(lower_kw) != std::string::npos) {
        return true;
    }

    if (to_lower_copy(artist_).find(lower_kw) != std::string::npos) {
        return true;
    }

    for (const auto& tag : tags_) {
        if (to_lower_copy(tag).find(lower_kw) != std::string::npos) {
            return true;
        }
    }

    return false;
}

// 输出操作符重载（核心修复：标签前加两个空格）
std::ostream& operator<<(std::ostream& os, const Song& s) {
    os << "[#" << s.id_ << "] " << s.artist_ << " - " << s.title_
       << " (" << s.duration_sec_ << "s) ";

    os << std::string(s.rating_, '*');

    // 关键修复：星号后添加两个空格，再显示标签
    if (!s.tags_.empty()) {
        os << "  [tags: " << join_tags(s.tags_) << "]";  // 两个空格
    }

    return os;
}

// 小于操作符重载
bool operator<(const Song& a, const Song& b) {
    if (a.rating_ != b.rating_) {
        return a.rating_ > b.rating_;
    }
    if (a.title_ != b.title_) {
        return a.title_ < b.title_;
    }
    return a.id_ < b.id_;
}

