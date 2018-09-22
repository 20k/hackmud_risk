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

struct result_info
{
    int idx = 0;
    int idy = 0;

    int result = 0;
};

result_info lcsubstr(const std::string& s1, const std::string& s2)
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

    result_info inf;

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
                //result = std::max(result, LCSuff[i][j]);

                if(LCSuff[i][j] > inf.result)
                {
                    inf.result = LCSuff[i][j];
                    inf.idx = i;
                    inf.idy = j;
                }
            }
            else LCSuff[i][j] = 0;
        }
    }
    return inf;
}

result_info overlap_strength(const std::string& s1, const std::string& s2)
{
    result_info inf = lcsubstr(s1, s2);

    if(inf.result != inf.idx && inf.result != inf.idy)
    {
        inf.result = 0;
    }

    return inf;
}

struct match
{
    result_info inf;

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

void merge_together(match& in)
{
    /*if(in.s2->data.find(in.s1->data) != std::string::npos)
    {
        in.s1->data = in.s2->data;
        return;
    }

    if(in.s1->data.find(in.s2->data) != std::string::npos)
    {
        in.s2->data = in.s1->data;
        return;
    }*/

    int which = -1;

    if(in.inf.idx != in.inf.result)
    {
        which = 0;
    }

    if(in.inf.idy != in.inf.result)
    {
        which = 1;
    }

    if(in.inf.idx != in.inf.result && in.inf.idy != in.inf.result)
    {
        throw std::runtime_error("Should not have happened");
    }

    if(which == 0)
    {
        for(int i=0; i < in.inf.result; i++)
        {
            in.s1->data.pop_back();
        }

        for(int i=0; i < (int)in.s2->data.size(); i++)
        {
            in.s1->data.push_back(in.s2->data[i]);
        }

        in.s2->data = in.s1->data;
    }

    if(which == 1)
    {
        for(int i=0; i < in.inf.result; i++)
        {
            in.s2->data.pop_back();
        }

        for(int i=0; i < (int)in.s1->data.size(); i++)
        {
            in.s2->data.push_back(in.s1->data[i]);
        }

        in.s1->data = in.s2->data;
    }
}

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

                    if(other->data == dat->data)
                        continue;

                    result_info strength = overlap_strength(dat->data, other->data);

                    if(strength.result > 0)
                    {
                        match m;
                        m.inf = strength;
                        m.s1 = dat;
                        m.s2 = other;
                        m.hit = false;

                        matches.push_back(m);
                    }
                }
            }

            std::sort(matches.begin(), matches.end(), [](const auto& i1, const auto& i2){return i1.inf.result > i2.inf.result;});

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
                fin.inf = i.inf;
                fin.s1 = i.s1;
                fin.s2 = i.s2;
                fin.hit = true;

                final_matches.push_back(fin);

                ///ok
                ///we should merge the matches together here
                ///and then recompute fully
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

    parsed_data s1;
    parsed_data s2;
    s1.data = "3456";
    s2.data = "1234";

    result_info test_inf = overlap_strength(s1.data, s2.data);

    match m;
    m.inf = test_inf;
    m.s1 = &s1;
    m.s2 = &s2;

    merge_together(m);

    std::cout << "res " << test_inf.result << " idx " << test_inf.idx << " y " << test_inf.idy << std::endl;

    std::cout << "pm " << m.s1->data << " m2 " << m.s2->data << std::endl;

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
