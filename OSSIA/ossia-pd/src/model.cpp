// A starter for Pd objects
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"

static t_eclass *model_class;

static void model_register(t_model *x)
{
    t_device* device = nullptr;

    if (!(device = (t_device*) find_parent(&x->x_obj,osym_device, 0))) return;

    // look for an [ossia.model] instance in the parent patchers
    t_model* model = find_parent_alive<t_model>(&x->x_obj,osym_model, 1);
    if (model)  {
        x->register_node(model->x_node);
        std::cout << "found a parent model" << std::endl;
        return;
    }
    std::cout << "found a parent device" << std::endl;
    x->register_node(device->x_node);
}

static void model_loadbang(t_model *x){
    std::cout << "[ossia.model] loadbang" << std::endl;
    model_register(x);
}

static void model_dump(t_model *x)
{
    t_atom a;
    std::string fullpath = get_absolute_path(x->x_node);
    SETSYMBOL(&a,gensym(fullpath.c_str()));
    outlet_anything(x->dumpout,gensym("fullpath"), 1, &a);
}

//FIXME : model is registered twice and parameter don't seem to be registered under model's node
bool t_model :: register_node(ossia::net::node_base*  node){
    std::cout << "[ossia.model] : register model : " << x_name->s_name << std::endl;

    x_node = nullptr;

    if (node){
        for (const auto& child : node->children()){
            if (child->getName() == x_name->s_name){
                x_node = child.get();
                break;
            }
        }
        if (!x_node) x_node = node->createChild(x_name->s_name);

        std::cout << "[ossia.model] : model node : " << x_node->getName() << std::endl;
        std::cout << "[ossia.model] : model node children count : " << std::hex << x_node->children().size() << std::endl;
    }

    std::vector<obj_hierachy> params = find_child(x_obj.o_canvas->gl_list, osym_param, 0);
    std::sort(params.begin(), params.end());
    for (auto v : params){
        t_parameter* param = (t_parameter*) v.x;
        param->register_node(x_node);
    }

    std::vector<obj_hierachy> remotes = find_child(x_obj.o_canvas->gl_list, osym_remote, 0);
    std::sort(remotes.begin(), remotes.end());
    for (auto v : remotes){
        t_remote* remote = (t_remote*) v.x;
        remote->register_node(x_node);
    }
    return true;
}

bool t_model :: unregister(){
    std::cout << "[ossia.model] : unregister model : " << x_name->s_name << std::endl;

    if(!x_node) return true; // not registered

    // when removing a model, we should re-register all its children to parent node
    std::vector<obj_hierachy> params = find_child(x_obj.o_canvas->gl_list, osym_param, 0);
    std::sort(params.begin(), params.end());
    for (auto v : params){
        t_parameter* param = (t_parameter*) v.x;
        if (!param->x_node || param->x_node->getParent() == x_node) param->register_node(x_node->getParent());
    }

    std::vector<obj_hierachy> remotes = find_child(x_obj.o_canvas->gl_list, osym_remote, 0);
    std::sort(remotes.begin(), remotes.end());
    for (auto v : remotes){
        t_remote* remote = (t_remote*) v.x;
        if (!remote->x_node || remote->x_node->getParent() == x_node) remote->register_node(x_node->getParent());
    }

    std::vector<obj_hierachy> models = find_child(x_obj.o_canvas->gl_list, osym_model, 0);
    std::sort(models.begin(), models.end());
    for (auto v : models){
        t_model* model = (t_model*) v.x;
        if (model != this && (!model->x_node || model->x_node->getParent() == x_node)) model->register_node(x_node->getParent());
    }

    x_node->getParent()->removeChild(x_name->s_name);
    x_node = nullptr;

    return true;
}

static void *model_new(t_symbol *name, int argc, t_atom *argv)
{
    t_model *x = (t_model *)eobj_new(model_class);
    std::cout << "[ossia.model] new instance: " << std::hex << x << std::endl;

    if(x)
    {
        x->x_absolute = false;
        x->x_node = nullptr;
        x->x_dead = false;
        x->dumpout = outlet_new((t_object*)x, gensym("dumpout"));

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (x->x_name != osym_empty && x->x_name->s_name[0] == '/') x->x_absolute = true;
        } else {
            x->x_name = gensym("untitledModel");
            pd_error(x,"You have to pass a name as the first argument");
        }
    }

    return (x);
}

static void model_free(t_model *x)
{
    x->x_dead = true;
    x->unregister();
    // FIXME : we do you want to register again here ?
    // x->register_node(nullptr);
}

extern "C" void setup_ossia0x2emodel(void)
{
    t_eclass *c = eclass_new("ossia.model", (method)model_new, (method)model_free, (short)sizeof(t_model), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) model_loadbang,   "loadbang",   A_NULL, 0);
        eclass_addmethod(c, (method) model_dump,       "dump",       A_NULL, 0);
    }

    model_class = c;
}