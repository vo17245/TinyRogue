-- color
BLACK={ 0.1, 0.1, 0.1, 1.0 }
BLUE={ 0.4, 0.5, 0.6, 1.0 }
GRAY={ 0.2, 0.2, 0.2, 1.0 }
GRAY2={ 0.32, 0.32, 0.32, 1.0 }
RED={ 0.8, 0.2, 0.2, 1.0 }
---------------
--- font
default_font="Assets/Fonts/Noto-Sans-S-Chinese/NotoSansHans-Regular-2.ttf"
--------------





local function value_or(value, default)
    if value == nil then
        return default
    else
        return value
    end
end
local function text(args)
    local id=args.id
    local width = args.width
    local height = args.height
    local text= args.text
    local font_size = value_or(args.font_size, 24)
    local font_color = value_or(args.font_color, { 1.0, 1.0, 1.0 })
    local font= value_or(args.font, default_font)
    return {
        id = id,
        type = "text",
        width = width,
        height = height,
        size = font_size,
        text = text,
        font_color = font_color,
        font=font,
    }

end
local function button(args)
    local width = args.width
    local height = args.height
    local tag = args.tag
    local color = args.color
    local id = args.id
    local font_size=value_or(args.font_size,24)
    local    font_color = value_or(args.font_color, { 1.0, 1.0, 1.0 })

    return {
        id = id,
        type = "quad",
        width = width,
        height = height,
        color = color,
        content = {
            text{
                type = "text",
                width = "100%",
                height = "100%",
                font_size = font_size,
                text = tag,
                font_color=font_color,

            }
        }
    }
end


local function file_button_panel(id,width,height)
    return {
        type="quad",
        width=width,
        height=height,
        visible=false,
        enable_layout=false,
        id=id,
        color=GRAY2,
        content={
            button { width = "100%", height = 32, tag = "Add Image", color = GRAY2, id = "btn:add_image" ,font_size=24},
            button { width = "100%", height = 32, tag = "Save", color = GRAY2, id = "btn:Save" ,font_size=24},
            button { width = "100%", height = 32, tag = "Load", color = GRAY2, id = "btn:Load" ,font_size=24},
        }
    }
end

return {
    type = "quad",
    width = "100%",
    height = "100%",
    color=GRAY,
    content = {
        button { width = "25%", height = 32, tag = "File", color = BLACK, id = "btn:file" ,font_size=24,font_color=WHITE},
        button { width = "25%", height = 32, tag = "Edit", color = BLACK, id = "btn:edit" ,font_size=24,font_color=WHITE},
        button { width = "25%", height = 32, tag = "Selection", color = BLACK, id = "btn:selection",font_size=24 ,font_color=WHITE},
        button { width = "25%", height = 32, tag = "View", color = BLACK, id = "btn:view" ,font_size=24,font_color=WHITE},
        file_button_panel("file_button_panel",150,32*4),
        {
            type="quad",
            id="atlas_view",
            width="100%",
        }


    },
}
