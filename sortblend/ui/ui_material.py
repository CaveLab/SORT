import bpy
from .. import common
from .. import nodes

class SORTMaterialPanel:
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "material"
    COMPAT_ENGINES = {common.default_bl_name}

    @classmethod
    def poll(cls, context):
        rd = context.scene.render
        return rd.engine in cls.COMPAT_ENGINES

class MaterialSlotPanel(SORTMaterialPanel, bpy.types.Panel):
    bl_label = common.material_slot_panel_bl_name

    def draw(self, context):
        layout = self.layout

        mat = context.material
        ob = context.object
        slot = context.material_slot
        space = context.space_data

        if ob:
            row = layout.row()

            row.template_list("MATERIAL_UL_matslots", "", ob, "material_slots", ob, "active_material_index", rows=4)

            col = row.column(align=True)

            col.operator("object.material_slot_add", icon='ZOOMIN', text="")
            col.operator("object.material_slot_remove", icon='ZOOMOUT', text="")

            if ob.mode == 'EDIT':
                row = layout.row(align=True)
                row.operator("object.material_slot_assign", text="Assign")
                row.operator("object.material_slot_select", text="Select")
                row.operator("object.material_slot_deselect", text="Deselect")

        split = layout.split(percentage=0.75)

        if ob:
            split.template_ID(ob, "active_material", new="material.new")
            row = split.row()

            if slot:
                row.prop(slot, "link", text="")
            else:
                row.label()
        elif mat:
            split.template_ID(space, "pin_id")
            split.separator()

class SORTPatternGraph(bpy.types.NodeTree):
    '''A node tree comprised of renderman nodes'''
    bl_idname = 'SORTPatternGraph'
    bl_label = 'SORT Pattern Graph'
    bl_icon = 'TEXTURE_SHADED'
    nodetypes = {}

    @classmethod
    def poll(cls, context):
        return context.scene.render.engine == common.default_bl_name

    # Return a node tree from the context to be used in the editor
    @classmethod
    def get_from_context(cls, context):
        ob = context.active_object
        if ob and ob.type not in {'LAMP', 'CAMERA'}:
            ma = ob.active_material
            if ma != None:
                nt_name = ma.sort_material.sortnodetree
                if nt_name != '':
                    return bpy.data.node_groups[nt_name], ma, ma
        return (None, None, None)

class SORT_use_shading_nodes(bpy.types.Operator):
    """Enable nodes on a material, world or lamp"""
    bl_idname = "sort.use_shading_nodes"
    bl_label = "Use Nodes"

    idtype = bpy.props.StringProperty(name="ID Type", default="material")

    @classmethod
    def poll(cls, context):
        return (getattr(context, "material", False) or getattr(context, "world", False) or
                getattr(context, "lamp", False))

    def execute(self, context):
        if context.material:
            context.material.sort_material.use_sort_nodes = True
        else:
            return

        mat = context.material
        idtype = self.properties.idtype
        context_data = {'material':context.material, 'lamp':context.lamp }
        idblock = context_data[idtype]

        nt = bpy.data.node_groups.new(idblock.name, type='SORTPatternGraph')
        nt.use_fake_user = True

        mat.sort_material.sortnodetree = nt.name
        #idblock.renderman.nodetree = nt.name
        output = nt.nodes.new('SORTOutputNode')
        default = nt.nodes.new('SORTLambertNode')
        default.location = output.location
        default.location[0] -= 300
        nt.links.new(default.outputs[0], output.inputs[0])
        return {'FINISHED'}

def find_node_input(node, name):
    for input in node.inputs:
        if input.name == name:
            return input
    return None

def find_node(material, nodetype):
    if material and material.sort_material and material.sort_material.sortnodetree:
        ntree = bpy.data.node_groups[material.sort_material.sortnodetree]

        active_output_node = None
        for node in ntree.nodes:
            if getattr(node, "type", None) == nodetype:
                if getattr(node, "is_active_output", True):
                    return node
                if not active_output_node:
                    active_output_node = node
        return active_output_node

    return None

def panel_node_draw(layout, id_data, output_type, input_name):
    if not id_data.sort_material.use_sort_nodes:
        layout.operator("sort.use_shading_nodes", icon='NODETREE')
        return False

    ntree = bpy.data.node_groups[id_data.sort_material.sortnodetree]

    node = find_node(id_data, output_type)
    if not node:
        layout.label(text="Empty Node")
    else:
        input = find_node_input(node, input_name)
        layout.template_node_view(ntree, node, input)

    return True

from .. import material

class SORTMaterialInstance(SORTMaterialPanel, bpy.types.Panel):
    bl_label = "Surface"

    @classmethod
    def poll(cls, context):
        return context.material and SORTMaterialPanel.poll(context)

    def draw(self, context):
        layout = self.layout
        mat = context.material

        if not panel_node_draw(layout, mat, 'CUSTOM', 'Surface'):
            layout.prop(mat, "diffuse_color")