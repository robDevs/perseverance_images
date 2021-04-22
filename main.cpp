#include "tools/curl-tools.h"
#include "tools/cJSON.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
int main(void)
{
    //first lets get the info about the rover. Max sols, launch/land date, etc. 
    std::string url = "https://api.nasa.gov/mars-photos/api/v1/rovers/?api_key=";
    printf("Fetching info about rovers...\n");
    std::string json = curl_get_string(url);

    printf("base: %s\n", url);

    const cJSON *max_sols = NULL;
    const cJSON *launch_date = NULL;
    const cJSON *land_date = NULL;
    const cJSON *total_photos = NULL;
    const cJSON *rover = NULL;

    printf("Parsing info about Perseverance rover...\n");
    cJSON *data = cJSON_Parse(json.c_str());
    if (data == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return 1;
    }

    const cJSON *rovers = cJSON_GetObjectItemCaseSensitive(data, "rovers");

    cJSON_ArrayForEach(rover, rovers) {
        const cJSON *name = cJSON_GetObjectItemCaseSensitive(rover, "name");
        std::string name_string = name->valuestring;
        if(name_string.compare("Perseverance") == 0) {
            max_sols = cJSON_GetObjectItemCaseSensitive(rover, "max_sol");
            launch_date = cJSON_GetObjectItemCaseSensitive(rover, "launch_date");
            land_date = cJSON_GetObjectItemCaseSensitive(rover, "land_date");
            total_photos = cJSON_GetObjectItemCaseSensitive(rover, "total_photos");
            break;
        }
    }
    printf("sols: %d\n", max_sols->valueint);

    //we have the needed info about the rover in question, let's start parsing it's data. 
    //data is broken up in sols since landing, and by pages of 25. 
    //need to parse each page for each sol. 
    int cur_sol = 1;
    int cur_page = 1;
    bool sol_done = false;
    
    std::string base_url = "https://api.nasa.gov/mars-photos/api/v1/rovers/Perseverance/photos?";
    std::string key = "api_key=";
    printf("base: %s", base_url);

    //let's open a file to write the html to
    std::ofstream outfile;
    outfile.open("perseverance_images.html", std::ios::trunc);

    if(!outfile.is_open()) {
        printf("Error opening outfile, aborting!!");
        return -1;
    }

    std::string base_html = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "<head>\n"
                            "<style>\n"
                                "body {\n"
                                "   background-color: #7d899e;\n"
                                "}\n"
                                ".center {\n"
                                "   margin: auto;\n"
                                "   width: 60%;\n"
                                "   background-color: #d4d4d4;\n"
                                "   box-shadow: 10px 10px 10px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);\n"
                                "   padding: 10px;\n"
                                "   text-align: center;\n"
                                "}\n"
                                "img {\n"
                                "   width: 50%;\n"
                                "   box-shadow: 10px 10px 10px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);\n"
                                "}\n"
                                "h1 {"
                                "   text-align: center;\n"
                                "}\n"
                            "</style>\n"
                            "</head>\n"
                            "<body>\n"
                            "    <h1>Perseverance Images</h1>\n"
                            "    <div class='center'>\n";
    
    outfile << base_html.c_str();
    for(cur_sol = max_sols->valueint; cur_sol > max_sols->valueint - 1; cur_sol--) {
        sol_done = false;
        cur_page = 1;
        while(sol_done == false) {
            //printf("sol: %d\npage: %d\n", cur_sol, cur_page);
            std::string cur_url = base_url;
            cur_url += "sol=" + std::to_string(cur_sol);
            cur_url += "&page=" + std::to_string(cur_page);
            cur_url += "&" + key;
            std::string cur_json = curl_get_string(cur_url);
            const cJSON *cur_data = cJSON_Parse(cur_json.c_str());
            const cJSON *photos = cJSON_GetObjectItemCaseSensitive(cur_data, "photos");
            if(cJSON_GetArraySize(photos) < 1) {
                sol_done = true;
                continue;
            }

            int count = 0;
            const cJSON *photo = NULL;
            cJSON_ArrayForEach(photo, photos) {
                const cJSON *src = cJSON_GetObjectItemCaseSensitive(photo, "img_src");
                const cJSON *earth_date = cJSON_GetObjectItemCaseSensitive(photo, "earth_date");
                const cJSON *cam_name = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItemCaseSensitive(photo, "camera"), "full_name");
                
                outfile << "    <p>Date: " << earth_date->valuestring << "<br>sol: " << cur_sol << "<br>Camera: " << cam_name->valuestring << "</p>\n";
                outfile << "    <a href='" << src->valuestring << "' target='_blank'><img src='";
                outfile << src->valuestring;
                outfile << "' alt='test'></a>\n";
            }
            cur_page += 1;
        }
    }

    base_html = "    </div>\n"
                "</body>\n"
                "</html>\n";
    outfile << base_html;

    outfile.close();
    return 0;
}