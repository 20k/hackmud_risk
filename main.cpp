#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <map>
#include <string_view>
#include <algorithm>

std::string read_file_bin(const std::string& file)
{
    std::ifstream t(file, std::ios::binary);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    return str;
}

std::vector<std::string> no_ss_split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

struct parsed_data
{
    std::string script_name;
    std::string data;
};

int lcsubstr(const std::string& s1, const std::string& s2)
{
    int m = s1.size();
    int n = s2.size();

    // Create a table to store lengths of longest common suffixes of
    // substrings.   Notethat LCSuff[i][j] contains length of longest
    // common suffix of X[0..i-1] and Y[0..j-1]. The first row and
    // first column entries have no logical meaning, they are used only
    // for simplicity of program
    //int LCSuff[m+1][n+1];
    std::vector<std::vector<int>> LCSuff;

    LCSuff.resize(m+1);

    for(auto& i : LCSuff)
        i.resize(n+1);

    int result = 0;  // To store length of the longest common substring

    /* Following steps build LCSuff[m+1][n+1] in bottom up fashion. */
    for (int i=0; i<=m; i++)
    {
        for (int j=0; j<=n; j++)
        {
            if (i == 0 || j == 0)
                LCSuff[i][j] = 0;

            else if (s1[i-1] == s2[j-1])
            {
                LCSuff[i][j] = LCSuff[i-1][j-1] + 1;
                result = std::max(result, LCSuff[i][j]);
            }
            else LCSuff[i][j] = 0;
        }
    }
    return result;
}


int overlap_strength(const std::string& s1, const std::string& s2)
{
    if(s2.find(s1) != std::string::npos)
        return s1.size();

    if(s1.find(s2) != std::string::npos)
        return s2.size();

    return lcsubstr(s1, s2);
}

struct match
{
    int strength = 0;

    parsed_data* s1 = nullptr;
    parsed_data* s2 = nullptr;

    bool hit = false;
};

#if 0
struct person
{
    parsed_data* me = nullptr;

    parsed_data* proposed_to = nullptr;

    int preference = 0;

    bool prefers_proposal(parsed_data* from, int strength)
    {
        if(strength > preference)
        {
            if(proposed_to != nullptr)
            {
                proposed_to->proposed_to = nullptr;
                proposed_to->preference = 0;
            }


            proposed_to = from;
            preference = strength;
            proposed_to->preference = strength;

            return true;
        }
    }
};

template<typename T>
std::vector<std::pair<T,T>> stable_roomate(const std::vector<T>& data)
{
    //std::map<parsed_data*, parsed_data*> best_matches;

    std::vector<person> people;


}
#endif // 0

struct parsed_data_manager
{
    std::vector<parsed_data*> data;

    std::map<std::string, std::vector<parsed_data*>> script_map;

    void add(parsed_data& dat)
    {
        parsed_data* n = new parsed_data(dat);

        data.push_back(n);
        script_map[n->script_name].push_back(n);
    }

    void dedup_scripts()
    {
        for(auto& p : script_map)
        {
            //std::vector<match> all_matches;

            std::vector<match> matches;

            std::vector<parsed_data*> all = p.second;

            for(int i=0; i < (int)all.size(); i++)
            {
                parsed_data* dat = all[i];

                for(int j=i+1; j < (int)all.size(); j++)
                {
                    parsed_data* other = all[j];

                    int strength = overlap_strength(dat->data, other->data);

                    if(strength > 0)
                    {
                        match m;
                        m.strength = strength;
                        m.s1 = dat;
                        m.s2 = other;
                        m.hit = false;

                        matches.push_back(m);
                    }
                }
            }

            std::sort(matches.begin(), matches.end(), [](const auto& i1, const auto& i2){return i1.strength > i2.strength;});

            ///just realised that its bidirectional
            ///so we just go down the preference list
            ///and then set them to fulfilled

            std::vector<match> final_matches;

            std::map<parsed_data*, bool> matched;

            for(auto& i : matches)
            {
                if(matched[i.s1])
                    continue;

                if(matched[i.s2])
                    continue;

                matched[i.s1] = true;
                matched[i.s2] = true;

                match fin;
                fin.strength = i.strength;
                fin.s1 = i.s1;
                fin.s2 = i.s2;
                fin.hit = true;

                final_matches.push_back(fin);
            }

            for(auto& i : final_matches)
            {
                //std::cout << "Matched " << i.s1->data << " with " << i.s2->data << std::endl;

                std::cout << "f1 " << i.s1->data << std::endl;
                std::cout << "f2 " << i.s2->data << std::endl;
            }

            /*for(auto& i : all_matches)
            {

            }*/

            ///ok so
            ///we want to compute the best match
        }

        std::cout << "hi\n";
    }
};

int main()
{
    std::string all_data = read_file_bin("data.txt");

    std::vector<std::string> post_split = no_ss_split(all_data, "\n");

    parsed_data_manager parsed_data_manage;

    for(std::string& str : post_split)
    {
        parsed_data dat;

        std::vector<std::string> split = no_ss_split(str, "$");

        assert(split.size() == 2);

        std::string script_name = no_ss_split(split[0], "@")[0];

        std::string text_name = split[1];

        dat.script_name = script_name;
        dat.data = text_name;

        parsed_data_manage.add(dat);
    }

    parsed_data_manage.dedup_scripts();

    return 0;
}
