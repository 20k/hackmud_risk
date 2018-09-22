#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <map>
#include <string_view>
#include <algorithm>
#include <set>

std::string read_file_bin(const std::string& file)
{
    std::ifstream t(file, std::ios::in);
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
    int which = -1;

    std::string substring;
};

result_info lcsubstr(const std::string& s1, const std::string& s2)
{
    int m = s1.size();
    int n = s2.size();

    //std::cout << m << n <<std::endl;

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

                if(LCSuff[i][j] >= inf.result)
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

/*bool starts_with(const std::string& s1, const std::string& s2)
{
    if(s1.size() < s2.size())
        return false;

    for(int i=0; i < (int)s1.size() ; i++)
    {
        if(s1[i] != s2[i])
            return false;
    }

    return true;
}*/

/*bool ends_with(const std::string& s1, const std::string& s2)
{
    if(s1.size() < s2.size())
        return false;

    int search_length = s2.size();
    int fin = (int)s1.size()-1;

    int terminate = fin - search_length;

    std::cout << "terminate on " << terminate << std::endl;
    std::cout << "str len " << s1.length() << std::endl;

    //for(int i=0; i < (int)s1.size() ; i++)
    for(int i = fin; i >= terminate && i >= 0; i--)
    {
        if(s1[i] != s2[i])
            return false;
    }

    return true;
}*/

struct match
{
    result_info inf;

    parsed_data* s1 = nullptr;
    parsed_data* s2 = nullptr;

    bool hit = false;
};

bool ends_with(result_info& inf, const std::string& s1, const std::string& s2)
{
    int rlen = inf.result;
    int idx = inf.idx;
    int idy = inf.idy;

    if(s1.find(s2) != std::string::npos)
        return true;

    if(s2.find(s1) != std::string::npos)
        return true;

    //const std::string& s1 = m.s1->data;
    //const std::string& s2 = m.s2->data;

    //std::cout << "bidx " << idx << " idy " << idy << " rlen " << rlen << std::endl;

    if(idx == 0 || idy == 0)
        return false;

    int base_idx = idx - rlen;
    int base_idy = idy - rlen;

    if(base_idx < 0 || base_idy < 0)
        return false;

    if((base_idx != 0 && base_idy != 0) || (base_idx + rlen != (int)s1.size() && base_idy + rlen != (int)s2.size()))
        return false;

    if(base_idx == 0 && base_idy == 0)
        return s1 == s2;

    int num = 0;

    //std::cout << "x " << base_idx << std::endl;
    //std::cout << "y " << base_idy << std::endl;

    for(int i=base_idx, j = base_idy; i < base_idx + rlen && i < (int)s1.size() && j < base_idy + rlen && j < (int)s2.size(); i++, j++)
    {
        //std::cout << "i " << i << " j " << j << std::endl;

        num++;

        //std::cout << "s1 " << s1[i] << " s2 " << s2[j] << std::endl;

        if(s1[i] != s2[j])
            return false;
    }

    //std::cout << "num " << num << std::endl;

    if(num == 0)
        return false;

    return true;
}

result_info overlap_strength(const std::string& s1, const std::string& s2)
{
    result_info inf = lcsubstr(s1, s2);

    //if(inf.result != inf.idx && inf.result != inf.idy)

    /*if(inf.idx != s1.size() && inf.idy != s1.size())
    {
        inf.result = 0;
    }*/

    //std::cout << "ends1 " << s1 << std::endl;
    //std::cout << "ends2 " << s2 << std::endl;

    /*std::cout << "rlen " << inf.result << std::endl;
    std::cout << "x " << inf.idx << std::endl;
    std::cout << "y " << inf.idy << std::endl;*/

    if(ends_with(inf, s1, s2))
    {
        inf.which = 0;

        int rlen = inf.result;
        int idx = inf.idx;
        int idy = inf.idy;

        //const std::string& s1 = m.s1->data;
        //const std::string& s2 = m.s2->data;

        std::string str;

        //for(int i=idx-1; i < (int)s1.size(); i++)

        /*for(int i=idx-1, j = idy - 1; i < (int)s1.size() && j < (int)s2.size(); i++, j++)
        {
            str.push_back(s1[i]);
        }*/

        int base_idx = idx - rlen;
        int base_idy = idy - rlen;

        for(int i=base_idx, j = base_idy; i < base_idx + rlen && j < base_idy + rlen; i++, j++)
        {
            str.push_back(s1[i]);
        }

        if(base_idx > 0 && base_idy > 0)
        {
            std::cout << "hi " << s1 << " s1" << std::endl;
            std::cout << "hi " << s2 << " s2" << std::endl;

            std::cout << "len " << rlen << std::endl;

            throw std::runtime_error("whelp");
        }

        //std::cout << "bidx " << base_idx << std::endl;

        std::string first;

        for(int i=0; i < base_idx; i++)
        {
            first.push_back(s1[i]);
        }

        for(int i=0; i < base_idy; i++)
        {
            first.push_back(s2[i]);
        }

        str = first + str;

        for(int i=base_idy + rlen; i < (int)s2.size(); i++)
        {
            str = str + s2[i];
        }

        for(int i=base_idx + rlen; i < (int)s1.size(); i++)
        {
            str = str + s1[i];
        }

        inf.substring = str;

        //std::cout << "got substr " << inf.substring << std::endl;
    }
    else
    {
        inf.result = 0;
        inf.which = -1;
    }

    return inf;
}

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

bool merge_together(match& in)
{
    //std::cout << "x " << in.inf.idx << " s " << in.s1->data.size() << " for " << in.s1->data << std::endl;
    //std::cout << "y " << in.inf.idy << " s " << in.s2->data.size() << " for " << in.s2->data << std::endl;

    //std::cout << "strn " << in.inf.result << std::endl;

    //if(in.inf.idx != in.inf.result)
    /*if(in.inf.idx != in.s1->data.size())
    {
        which = 0;
    }

    //if(in.inf.idy != in.inf.result)
    if(in.inf.idy != in.s2->data.size())
    {
        which = 1;
    }*/

    int which = in.inf.which;

    if(which == -1)
        return false;

    /*if(in.inf.idx != in.inf.result && in.inf.idy != in.inf.result)
    {
        throw std::runtime_error("Should not have happened");
    }*/

    //std::cout << "merg " << in.s1->data << std::endl;
    //std::cout << "into " << in.s2->data << std::endl;

    /*if(which == 0)
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
    }*/

    in.s1->data = in.inf.substring;
    in.s2->data = in.inf.substring;

    return true;
}

std::vector<match> sort_overlap(const std::vector<parsed_data*>& all, std::map<std::string, std::map<std::string, bool>>& does_not_overlap)
{
    std::vector<match> matches;

    std::map<std::string, bool> any_seen;
    std::map<std::string, bool> any_passed;

    for(int i=0; i < (int)all.size(); i++)
    {
        parsed_data* dat = all[i];

        for(int j=i+1; j < (int)all.size(); j++)
        {
            parsed_data* other = all[j];

            any_seen[dat->data] = true;
            any_seen[other->data] = true;

            if(other == dat)
                continue;

            if(other->data == dat->data)
                continue;

            if(does_not_overlap[dat->data][other->data])
                continue;

            //std::cout << "hi " << other->data << std::endl;

            result_info strength = overlap_strength(dat->data, other->data);

            if(strength.result > 0)
            {
                any_passed[dat->data] = true;
                any_passed[other->data] = true;

                match m;
                m.inf = strength;
                m.s1 = dat;
                m.s2 = other;
                m.hit = false;

                matches.push_back(m);
            }
            else
            {
                does_not_overlap[dat->data][other->data] = true;
            }
        }
    }

    std::sort(matches.begin(), matches.end(), [](const auto& i1, const auto& i2){return i1.inf.result > i2.inf.result;});

    /*if(matches.size() == 0 && all.size() > 0)
    {
        match m;
        m.s1 = all[0];
        m.s2 = all[0];

        matches.push_back(m);
    }*/

    return matches;
}

std::vector<parsed_data*> get_unique(const std::vector<parsed_data*>& all)
{
    std::set<std::string> ret;

    for(int i=0; i < (int)all.size(); i++)
    {
        parsed_data* dat = all[i];

        ret.insert(dat->data);
    }

    std::vector<parsed_data*> rv;

    /*for(auto& i : all)
    {
        if(ret.find(i->data) != ret.end())
        {
            rv.push_back(i);
        }
    }*/

    for(auto& i : ret)
    {
        for(auto& k : all)
        {
            if(i == k->data)
            {
                rv.push_back(k);
                break;
            }
        }
    }

    return rv;
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

            bool should_go = true;

            //for(int num = 0; num < p.second.size()*5; num++)

            std::map<std::string, std::map<std::string, bool>> does_not_overlap;

            while(should_go)
            {
                std::vector<match> matches = sort_overlap(p.second, does_not_overlap);

                //std::cout << "start matches " << matches.size() << std::endl;

                should_go = false;

                //if(matches.size() > 1)
                for(int i=0; i < (int)matches.size(); i++)
                {
                    match& m = matches[i];

                    //std::cout << " merg " << m.s1->data << std::endl;
                    //std::cout << " into " << m.s2->data << std::endl;

                    if(!merge_together(matches[i]))
                        continue;

                    should_go = true;

                    //std::cout <<" gote " << m.s1->data << std::endl;
                    break;
                }

                //if(matches.size() <= 1)
                //    break;
            }

            //std::vector<match> matches = sort_overlap(p.second);

            std::vector<parsed_data*> dat = get_unique(p.second);

            std::cout << "msize " << dat.size() << std::endl;

            for(auto& i : dat)
            {
                //std::cout << "Matched " << i.s1->data << " with " << i.s2->data << std::endl;

                std::cout << "f1 " << i->data << std::endl;
                //std::cout << "f2 " << i.s2->data << std::endl;
            }

            ///just realised that its bidirectional
            ///so we just go down the preference list
            ///and then set them to fulfilled

            /*std::vector<match> final_matches;

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
            }*/

            /*for(auto& i : all_matches)
            {

            }*/

            ///ok so
            ///we want to compute the best match
        }

        std::cout << "hi\n";
    }
};

std::string full_test(const std::string& us1, const std::string& us2)
{
    parsed_data s1;
    parsed_data s2;

    s1.data = us1;
    s2.data = us2;

    result_info test_inf = overlap_strength(s1.data, s2.data);

    match m;
    m.inf = test_inf;
    m.s1 = &s1;
    m.s2 = &s2;

    merge_together(m);

    return test_inf.substring;
}

void tests()
{
    /*assert(full_test("over1234", "1234") == "over1234");
    assert(full_test("over1234", "12234").size() == 0);
    assert(full_test("1234", "3456") == "123456");

    assert(full_test("gineering problem - them - they're", "n asked to so they're thinking like engineering problem") == "n asked to so they're thinking like engineering problem - them - they're");*/

    assert(full_test(", and then comes the things", ", and then comes the things they s") == ", and then comes the things they s");

    /*ogs_playin
ogs_playing? ## Not f*/

    assert(full_test("ogs_playin", "ogs_playing? ## Not f") == "ogs_playing? ## Not f");

    //. Petra permitt
    //rators. Petra permitt

    assert(full_test(". Petra permitt", "rators. Petra permitt") == "rators. Petra permitt");

    //std::cout << "pmerge " << full_test("1234", "3456") << " well" << std::endl;
}

int main()
{
    tests();

    std::string all_data = read_file_bin("data.txt");

    std::vector<std::string> post_split = no_ss_split(all_data, "\n");

    parsed_data s1;
    parsed_data s2;
    //s1.data = "n asked to so they're thinking like engineering";
    //s2.data = "thinking like engineering problem";

    //s1.data = "like engineering problem";
    //s2.data = "ineering problem - they've been asked to so";

    /*oving?
    oves AI*/

    /*n asked to so they're thinking like engineering
    thinking like engineering problem*/

    /*s1.data = "over1234a";
    s2.data = "1234over1234";*/

    /*n asked to so they're thinking like engineering problem - they've been asked to so
gineering problem - them - they're*/

    s1.data = "n asked to so they're thinking like engineering problem - they've been asked to so";
    s2.data = "gineering problem - them - they're";

    /*gineering problem - them - they're
n asked to so they're thinking like engineering problem*/

    result_info test_inf = overlap_strength(s1.data, s2.data);

    std::cout << ends_with(test_inf, s1.data, s2.data) << " ewith\n";

    match m;
    m.inf = test_inf;
    m.s1 = &s1;
    m.s2 = &s2;

    merge_together(m);

    std::cout << "res " << test_inf.result << " idx " << test_inf.idx << " y " << test_inf.idy << std::endl;

    std::cout << "pm " << m.s1->data << " m2 " << m.s2->data << std::endl;


    //return 0;


    /* ineering problem - they've been asked to so
    like engineering problem*/

    //return ends_with(m);

    //std::cout << ends_with("like engineering problem", "ineering problem - they've been asked to so") << std::endl;

    //return 0;

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
