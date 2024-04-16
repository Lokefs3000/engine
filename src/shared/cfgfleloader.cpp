#include "../../src/pch.h"
#include "cfgfleloader.h"

#include <spdlog/spdlog.h>
#define warn(...) spdlog::get("SHARED")->warn(__VA_ARGS__)
#define error(...) spdlog::get("SHARED")->error(__VA_ARGS__)

#include "strutil.h"

enum class cfg_node_btype : uint8_t
{
    _object,
    _array,
    _int,
    _bool,
    _float,
    _string,

    _objend,

    _unkown
};

int cfg_node::as_int()
{
    return m_int;
}

float cfg_node::as_float()
{
    return m_float;
}

bool cfg_node::as_bool()
{
    return m_bool;
}

std::string cfg_node::as_str()
{
    return m_str;
}

cfg_node& cfg_node::at(size_t i)
{
    if (m_array.size() < i)
        return m_array[i];
    cfg_node node{};
    return node;
}

cfg_node& cfg_node::at(std::string i)
{
    if (m_nodes.contains(i))
        return m_nodes.at(i);
    cfg_node node{};
    return node;
}

cfg_node& cfg_node::add(std::string name, int v)
{
    m_nodes.insert(std::make_pair(name, cfg_node{}));
    cfg_node& node = m_nodes.at(name);
    node.m_type = cfg_node_btype::_int;
    node.m_int = v;
    return node;
}

cfg_node& cfg_node::add(std::string name, float v)
{
    m_nodes.insert(std::make_pair(name, cfg_node{}));
    cfg_node& node = m_nodes.at(name);
    node.m_type = cfg_node_btype::_float;
    node.m_float = v;
    return node;
}

cfg_node& cfg_node::add(std::string name, bool v)
{
    m_nodes.insert(std::make_pair(name, cfg_node{}));
    cfg_node& node = m_nodes.at(name);
    node.m_type = cfg_node_btype::_bool;
    node.m_bool = v;
    return node;
}

cfg_node& cfg_node::add(std::string name, std::string v)
{
    m_nodes.insert(std::make_pair(name, cfg_node{}));
    cfg_node& node = m_nodes.at(name);
    node.m_type = cfg_node_btype::_string;
    node.m_str = v;
    return node;
}

cfg_node& cfg_node::add(std::string name, cfg_node v)
{
    m_nodes.insert(std::make_pair(name, v));
    return m_nodes.at(name);
}

std::map<std::string, cfg_node>& cfg_node::nodes()
{
    return m_nodes;
}

cfg_node_btype cfg_node::type()
{
    return m_type;
}

static const char* cfg_node_btype_map[] = {
    "object",
    "array",
    "int",
    "bool",
    "float",
    "string",

    "objend",

    "unkown"
};

cfg_node_btype determine_bvaltype(std::string val)
{
    if (val.starts_with("[") && val.ends_with("]"))
        return cfg_node_btype::_array;
    else if (val.starts_with("\"") && val.ends_with("\""))
        return cfg_node_btype::_string;

    std::string low = strutil::lower(val);
    if (low == "true" || low == "false")
        return cfg_node_btype::_bool;

    int digits = 0;
    for (size_t i = (int)val.starts_with("-"); i < val.size(); i++)
        digits += (int)(std::isdigit(val[i]) > 0);

    if (digits == val.size() - 1 && val.starts_with("-"))
        digits++;

    if (digits == val.size() - 1 && val.ends_with("f"))
        return cfg_node_btype::_float;
    else if (digits == val.size())
        return cfg_node_btype::_int;
    
    return cfg_node_btype::_unkown;
}

void cfgfileloader::configure_new_node(cfg_node*& ref, std::string& line, std::string& tval, std::string& tname, size_t& i, cfg_node_btype& btype)
{
    switch (btype)
    {
    case cfg_node_btype::_object:
    {
        std::string name = line.substr(0, line.size() - 1);
        ref->m_nodes.insert(std::make_pair(name, cfg_node{}));
        cfg_node& obj = ref->m_nodes.at(name);
        obj.m_str = name;
        obj.m_parent = ref;
        obj.m_type = btype;

        ref = &obj;
    }
    break;
    case cfg_node_btype::_array:
    {
        cfg_node val{};
        val.m_type = btype;
        val.m_parent = ref;

        cfg_node* pval = &val;

        std::string new_val = tval.substr(1, tval.size() - 2);
        std::istringstream line_str(new_val);
        std::string arr_element;

        while (std::getline(line_str, arr_element, ','))
        {
            std::string lval = strutil::trimfb(arr_element);
            cfg_node_btype tbtype = determine_bvaltype(lval);
            
            configure_new_node(pval, line, lval, tname, i, btype);
        }

        if (ref->m_type == cfg_node_btype::_object)
            ref->m_nodes.insert(std::make_pair(tname, val));
        else
            ref->m_array.push_back(val);
    }
    break;
    case cfg_node_btype::_int:
    {
        cfg_node val{};
        val.m_int = std::stoi(tval);
        val.m_type = btype;
        val.m_parent = ref;
        if (ref->m_type == cfg_node_btype::_object)
            ref->m_nodes.insert(std::make_pair(tname, val));
        else
            ref->m_array.push_back(val);
    }
    break;
    case cfg_node_btype::_bool:
    {
        cfg_node val{};
        val.m_bool = (tval == "true");
        val.m_type = btype;
        val.m_parent = ref;
        if (ref->m_type == cfg_node_btype::_object)
            ref->m_nodes.insert(std::make_pair(tname, val));
        else
            ref->m_array.push_back(val);
    }
    break;
    case cfg_node_btype::_float:
    {
        cfg_node val{};
        val.m_float = std::stof(tval);
        val.m_type = btype;
        val.m_parent = ref;
        if (ref->m_type == cfg_node_btype::_object)
            ref->m_nodes.insert(std::make_pair(tname, val));
        else
            ref->m_array.push_back(val);
    }
    break;
    case cfg_node_btype::_string:
    {
        cfg_node val{};
        val.m_str = tval.substr(1, tval.size() - 2);
        val.m_type = btype;
        val.m_parent = ref;
        if (ref->m_type == cfg_node_btype::_object)
            ref->m_nodes.insert(std::make_pair(tname, val));
        else
            ref->m_array.push_back(val);
    }
    break;
    case cfg_node_btype::_objend:
    {
        ref = ref->m_parent;
    }
    break;
    case cfg_node_btype::_unkown:
    default:
        warn("unkown line at: {}:{}, line: \"{}\"", 0, i, line);
        break;
    }
}

cfg_node cfgfileloader::load(std::string& string)
{
    cfg_node root{};
    cfg_node* ref = &root;

    std::vector<std::string> lines = strutil::split(string, '\n');
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string line = strutil::trimfb(lines[i]);
        if (line.empty())
            continue;

        cfg_node_btype btype = cfg_node_btype::_unkown;

        std::string tname = "";
        std::string tval = "";

        size_t find = line.find_first_of(":");
        if (line.size() == 1 && line[0] == '}' /*"}" puts a \0 at the end >:(*/)
            btype = cfg_node_btype::_objend;
        else if (line.find_last_of("{") == line.size() - 1 && line.ends_with("{"))
            btype = cfg_node_btype::_object;
        else if (find != std::string::npos)
        {
            tval = line.substr(find + 1);
            tname = line.substr(0, find);

            tval = strutil::trimfb(tval); tname = strutil::trimfb(tname);

            if (tval.empty())
            {
                warn("value in kvp not found at: {}:{}, line: \"{}\"", find, i, line);
                continue;
            }

            btype = determine_bvaltype(strutil::trimfb(tval));
        }
        else
        {
            warn("line does not contain an object or a kvp: {}:{}, line: \"{}\"", line.size(), i, line);
        }

        configure_new_node(ref, line, tval, tname, i, btype);
    }

    return root;
}

void serialize_node(std::string& src, size_t recursion, std::string name, cfg_node& node)
{
    std::string padding(recursion * 4, ' ');

    switch (node.type())
    {
    case cfg_node_btype::_object:
    {
        src += padding + name + "{\n";
        for (std::pair<const std::string, cfg_node>& child : node.nodes())
            serialize_node(src, recursion + 1, child.first, child.second);
        src += padding + "}\n";
    }
    break;
    case cfg_node_btype::_array:
        break;
    case cfg_node_btype::_int:
    {
        src += padding + name + ": " + std::to_string(node.as_int()) + "\n";
    }
    break;
    case cfg_node_btype::_float:
    {
        src += padding + name + ": " + std::to_string(node.as_float()) + "\n";
    }
    break;
    case cfg_node_btype::_bool:
    {
        src += padding + name + ": " + (node.as_bool() ? "true" : "false") + "\n";
    }
    break;
    case cfg_node_btype::_string:
    {
        src += padding + name + ": \"" + node.as_str() + "\"\n";
    }
    break;
    case cfg_node_btype::_unkown:
    default:
        warn("unkown node type: {}", cfg_node_btype_map[(int)node.type()]);
        break;
    }
}

std::string cfgfileloader::save(cfg_node& root)
{
    if (root.m_type != cfg_node_btype::_object)
    {
        error("cannot save root node because it is not an object! actual type: ", cfg_node_btype_map[(int)root.m_type]);
        return "";
    }

    std::string src = "";
    for (std::pair<const std::string, cfg_node>& node : root.nodes())
        serialize_node(src, 0, node.first, node.second);
    return src.substr(0, std::min(src.size() - 1, src.size()));
}
