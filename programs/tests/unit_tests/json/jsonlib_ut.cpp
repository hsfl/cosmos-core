#include "jsonlib_ut.h"

namespace Cosmos {
namespace Test {
namespace Unit {
namespace Json {


void JsonlibUTest::SetUp()
{
    // Clear previously created test directory, if any
    rmdir(get_cosmosnodes() + TEST_NODE_NAME);

    srand(decisec());
    agent = new Agent(0);
    // Initialize COSMOS data space
    agent->cinfo = json_init();
    ASSERT_NE(agent->cinfo, nullptr);
    agent->cinfo->node.name = TEST_NODE_NAME;
}

// Cleanup for text fixture class
void JsonlibUTest::TearDown()
{
    //rmdir(get_cosmosnodes() + TEST_NODE_NAME);
    // agent
    if (agent != nullptr)
    {
        delete agent->cinfo;
        agent->cinfo = nullptr;
        delete agent;
    }
    // agent2
    if (agent2 != nullptr)
    {
        delete agent2->cinfo;
        agent2->cinfo = nullptr;
        delete agent2;
    }
}

TEST_F(JsonlibUTest, Fixture_class_initializes_correctly)
{
    SetUp();
}

TEST_F(JsonlibUTest, ini_dump_loads_correctly)
{
    SetUp();

    // Create all devices
    make_devices();
    
    // Create a new agent that reads in the values
    agent2 = new Agent(0);

    // Initialize COSMOS data space
    agent2->cinfo = json_init();
    ASSERT_NE(agent2->cinfo, nullptr);
    agent2->cinfo->node.name = TEST_NODE_NAME;
    int32_t iretn = json_setup_node(agent2->cinfo->node.name, agent2->cinfo);
    ASSERT_EQ(iretn, 0);
    
    // Node
    EXPECT_EQ(agent->cinfo->node.type, agent2->cinfo->node.type);
    ASSERT_EQ(agent->cinfo->node.piece_cnt, agent2->cinfo->node.piece_cnt);
    ASSERT_EQ(agent->cinfo->node.vertex_cnt, agent2->cinfo->node.vertex_cnt);
    ASSERT_EQ(agent->cinfo->node.face_cnt, agent2->cinfo->node.face_cnt);
    ASSERT_EQ(agent->cinfo->node.device_cnt, agent2->cinfo->node.device_cnt);
    ASSERT_EQ(agent->cinfo->node.port_cnt, agent2->cinfo->node.port_cnt);

    // Vertices
    for (uint16_t i=0; i<agent->cinfo->node.vertex_cnt; ++i)
    {
        compare_Vector(agent->cinfo->node.phys.vertices[i], agent2->cinfo->node.phys.vertices[i], __LINE__);
    }

    // Faces
    for (uint16_t i=0; i<agent->cinfo->node.face_cnt; ++i)
    {
        compare_Vector(agent->cinfo->node.phys.faces[i].normal, agent2->cinfo->node.phys.faces[i].normal, __LINE__);
        compare_Vector(agent->cinfo->node.phys.faces[i].com, agent2->cinfo->node.phys.faces[i].com, __LINE__);
        EXPECT_DOUBLE_EQ(agent->cinfo->node.phys.faces[i].area, agent2->cinfo->node.phys.faces[i].area);
        EXPECT_EQ(agent->cinfo->node.phys.faces[i].vertex_cnt, agent2->cinfo->node.phys.faces[i].vertex_cnt);
        if (agent->cinfo->node.phys.faces[i].vertex_cnt != agent2->cinfo->node.phys.faces[i].vertex_cnt)
        {
            break;
        }
        for (uint16_t j=0; j<agent->cinfo->node.phys.faces[i].vertex_cnt; ++j)
        {
            EXPECT_EQ(agent->cinfo->node.phys.faces[i].vertex_idx[j], agent2->cinfo->node.phys.faces[i].vertex_idx[j]) << "Error in i: " << i << " j: " << j;
        }
    }

    // Pieces
    for (uint16_t i=0; i<agent->cinfo->node.piece_cnt; ++i)
    {
        EXPECT_EQ(agent->cinfo->pieces[i].name,          agent2->cinfo->pieces[i].name);
        EXPECT_EQ(agent->cinfo->pieces[i].cidx,          agent2->cinfo->pieces[i].cidx);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].mass,    agent2->cinfo->pieces[i].mass);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].density, agent2->cinfo->pieces[i].density);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].emi,     agent2->cinfo->pieces[i].emi);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].abs,     agent2->cinfo->pieces[i].abs);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].hcap,    agent2->cinfo->pieces[i].hcap);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].hcon,    agent2->cinfo->pieces[i].hcon);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].dim,     agent2->cinfo->pieces[i].dim);
        EXPECT_FLOAT_EQ(agent->cinfo->pieces[i].area,    agent2->cinfo->pieces[i].area);
        compare_Vector( agent->cinfo->pieces[i].com,     agent2->cinfo->pieces[i].com, __LINE__);
        EXPECT_EQ(agent->cinfo->pieces[i].face_cnt,      agent2->cinfo->pieces[i].face_cnt);
        if (agent->cinfo->pieces[i].face_cnt != agent2->cinfo->pieces[i].face_cnt)
        {
            break;
        }
        for (uint16_t j=0; j<agent->cinfo->pieces[i].face_cnt; ++j)
        {
            EXPECT_EQ(agent->cinfo->pieces[i].face_idx[j], agent->cinfo->pieces[i].face_idx[j]) << "Error in i: " << i << " j: " << j;
        }
    }

    // General Devices
    for (uint16_t i=0; i<agent->cinfo->node.device_cnt; ++i)
    {
        EXPECT_EQ(agent->cinfo->device[i]->type, agent2->cinfo->device[i]->type);
        EXPECT_EQ(agent->cinfo->device[i]->name, agent2->cinfo->device[i]->name);
        EXPECT_EQ(agent->cinfo->device[i]->model, agent2->cinfo->device[i]->model);
        EXPECT_EQ(agent->cinfo->device[i]->didx, agent2->cinfo->device[i]->didx);
        EXPECT_EQ(agent->cinfo->device[i]->pidx, agent2->cinfo->device[i]->pidx) << "Error in i: " << i;
        EXPECT_EQ(agent->cinfo->device[i]->bidx, agent2->cinfo->device[i]->bidx);
        EXPECT_EQ(agent->cinfo->device[i]->addr, agent2->cinfo->device[i]->addr);
        EXPECT_EQ(agent->cinfo->device[i]->portidx, agent2->cinfo->device[i]->portidx);
        EXPECT_FLOAT_EQ(agent->cinfo->device[i]->nvolt, agent2->cinfo->device[i]->nvolt);
        EXPECT_FLOAT_EQ(agent->cinfo->device[i]->namp, agent2->cinfo->device[i]->namp);
        EXPECT_EQ(agent->cinfo->device[i]->flag, agent2->cinfo->device[i]->flag);
    }

    // Specific Devices
    string devspec, devspec2;
    json_devices_specific(devspec, agent->cinfo);
    json_devices_specific(devspec2, agent2->cinfo);
    EXPECT_EQ(devspec, devspec2);

    // Ports
    for (uint16_t i=0; i<agent->cinfo->node.port_cnt; ++i)
    {
        EXPECT_EQ(agent->cinfo->port[i].name, agent2->cinfo->port[i].name);
        EXPECT_EQ(agent->cinfo->port[i].type, agent2->cinfo->port[i].type);
    }

    // Aliases
    EXPECT_EQ(agent->cinfo->alias.size(), agent2->cinfo->alias.size());
    EXPECT_EQ(agent->cinfo->emap.size(), agent2->cinfo->emap.size());
    if (agent->cinfo->alias.size() == agent2->cinfo->alias.size()
    && agent->cinfo->emap.size() == agent2->cinfo->emap.size())
    {
        auto ait = agent->cinfo->alias.begin();
        auto ait2 = agent2->cinfo->alias.begin();
        for (;ait != agent->cinfo->alias.end();)
        {
            EXPECT_EQ(agent->cinfo->emap[ait->handle.hash].size(), agent2->cinfo->emap[ait2->handle.hash].size());
            if (agent->cinfo->emap[ait->handle.hash].size() != agent2->cinfo->emap[ait2->handle.hash].size())
            {
                break;
            }
            EXPECT_EQ(agent->cinfo->emap[ait->handle.hash][ait->handle.index].text, agent2->cinfo->emap[ait2->handle.hash][ait2->handle.index].text);
            ait++;
            ait2++;
        }
    }

    // Equations
    EXPECT_EQ(agent->cinfo->equation.size(), agent2->cinfo->equation.size());
    if (agent->cinfo->equation.size() == agent2->cinfo->equation.size())
    {
        for (size_t i=0; i<agent->cinfo->equation.size(); ++i)
        {
            EXPECT_EQ(agent->cinfo->equation[i].name, agent2->cinfo->equation[i].name);
        }
    }

}


} // End namespace Channel
} // End namespace Unit
} // End namespace Test
} // End namespace Cosmos
