// Define the lowest year that the picker can select. In this example, '1970' is the Unix epoch.
#define IMGUI_DATEPICKER_YEAR_MIN 1970
// Define the highest year that the picker can select.
#define IMGUI_DATEPICKER_YEAR_MAX 2500
#include "ImGuiDatePicker.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "new_event.h"
#include <chrono>
#include <libical/ical.h>
bool NewEvent::makeNew = false;

void NewEvent::makeEvent() {
    if (!makeNew) {
        return;
    }
    ImGui::OpenPopup("Create New Event");
    if (ImGui::BeginPopupModal("Create New Event"))
    {
        static char eventName[128] = "";
        static char eventLocation[128] = "";
        static int startHour = 0;
        static int startMinute = 0;
        static int endHour = 23;
        static int endMinute = 59;
        static int duration = 0;
        //static int sampm = 0;
        //static int eampm = 0;
        static int repeatable = 0;

        ImGui::Text("Event Name");
        ImGui::InputText("##EventName", eventName, sizeof(eventName));
        ImGui::Text("Location");
        ImGui::InputText("##Location", eventLocation, sizeof(eventLocation));
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        tm t = *std::gmtime(&currentTime);
        static tm setDate = t;
        ImGui::Text("Date");
        // Use the picker for date
        if (ImGui::DatePicker("", setDate))
        {
            // Perform some event whenever the date 't' is changed
        }
        // Start Time
        NewEvent::formatTime(&startHour, &startMinute, &endHour, &endMinute, &duration); //ImGui::SameLine();
        //NewEvent::formatTime(&endHour, &endMinute, &eampm); Helpful if we can make multiple calls to same function


        // Repeat Event
        ImGui::Text("Repeat");
        ImGui::RadioButton("Never", &repeatable, 0); ImGui::SameLine();
        ImGui::RadioButton("Daily", &repeatable, 1); ImGui::SameLine();
        ImGui::RadioButton("Weekly", &repeatable, 2); ImGui::SameLine();
        ImGui::RadioButton("Monthly", &repeatable, 3); ImGui::SameLine();
        ImGui::RadioButton("Yearly", &repeatable, 4);

        //NewEvent event = (eventName, eventLocation, currentHour, currentMinute); // For ICal

        if (ImGui::Button("Create")) {
            // Put event on calendar here (currently sends to txt for test)
            convertICal(eventName, eventLocation, &startHour, &startMinute, &duration, &setDate);
            // Close popup
            ImGui::CloseCurrentPopup();
            NewEvent::setEvent(false);
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            NewEvent::setEvent(false);
        }
        ImGui::EndPopup();
    }
}

void NewEvent::formatTime(int *hour, int *minute, int *ehour, int *eminute, int *duration) {
    const char* hours[24] = {};
    const char* minutes[60] = {};

    ImGui::Text("Start Time"); ImGui::SameLine();
    ImGui::PushItemWidth(80);
    // Event Start Time
    // Start Hour (ideally current hour but issues arising atm)
    for (int i = 0; i < 24; i++) {
        char* hourStr = new char[3];
        snprintf(hourStr, 3, "%02d", i);
        hours[i] = hourStr;
    }
    ImGui::ListBox("##Hour", hour, hours, IM_ARRAYSIZE(hours), 4); ImGui::PushItemWidth(80); ImGui::SameLine();
    for (int i = 0; i < 24; i++) {
        delete[] hours[i]; // Free the memory for each hour string
    }

    // Start Minutes
    for (int i = 0; i < 60; i++) {
        char* minuteStr = new char[3];
        snprintf(minuteStr, 3, "%02d", i);
        minutes[i] = minuteStr;
    }
    ImGui::ListBox("##Minute", minute, minutes, IM_ARRAYSIZE(minutes), 4); ImGui::PushItemWidth(80); ImGui::SameLine();
    for (int i = 0; i < 60; i++) {
        delete[] minutes[i]; // Free the memory for each minute string
    }

    // Event End Time
    ImGui::Text("End Time"); ImGui::SameLine();
    ImGui::PushItemWidth(80);
    // End Hour
    for (int i = 0; i < 24; i++) {
        char* hourStr = new char[3];
        snprintf(hourStr, 3, "%02d", i);
        hours[i] = hourStr;
    }
    ImGui::ListBox("##1Hour", ehour, hours, IM_ARRAYSIZE(hours), 4); ImGui::PushItemWidth(80); ImGui::SameLine();
    for (int i = 0; i < 24; i++) {
        delete[] hours[i]; // Free the memory for each hour string
    }

    // End Minutes
    for (int i = 0; i < 60; i++) {
        char* minuteStr = new char[3];
        snprintf(minuteStr, 3, "%02d", i);
        minutes[i] = minuteStr;
    }
    ImGui::ListBox("##2Minute", eminute, minutes, IM_ARRAYSIZE(minutes), 4); ImGui::PushItemWidth(80);
    for (int i = 0; i < 60; i++) {
        delete[] minutes[i]; // Free the memory for each minute string
    }

    /* AM / PM Picker(if we return to non - military time)
    ImGui::RadioButton("AM##1", &eampm, 0); ImGui::SameLine();
    ImGui::RadioButton("PM##1", &eampm, 1); */

    // calculate duration (in minutes?)
    int startTotal = (*hour * 60) + *minute;
    int endTotal = (*ehour * 60) + *eminute;
    // for spanning multiple days (not possible yet)
    if (endTotal <= startTotal) {
        endTotal += 24 * 60; // Add 24 hours worth of minutes
    }
    *duration = endTotal - startTotal;
}

void NewEvent::convertICal(char* name, char* locate, int* shour, int* sminute, int* duration, tm *date){
   // icalcomponent* cal, * event;
   // icalproperty* prop;
   // const char* my_product, * uid, * tzid, * desc;
    //struct icaltimetype dtstart;
    //struct icaldurationtype iduration;

    /* Create VCALENDAR component */
   // cal = icalcomponent_new(ICAL_VCALENDAR_COMPONENT);

    /* Create start time for our meeting
    dtstart = icaltime_null_time();
    dtstart.year = date->tm_year;
    dtstart.month = date->tm_mon;
    dtstart.day = date->tm_wday;
    dtstart.hour = *shour;
    dtstart.minute = *sminute;
    int dtduration = *duration;
    // tzid = "America/New_York";
    /* Create duration for our meeting (1hr = 3600s) */
    //iduration = icaldurationtype_from_int(dtduration);

    /* Create VEVENT component.
       In this case, we use the varargs variants of creating a component
       and property which allows us to construct the component all in one go.
    event =
        icalcomponent_vanew(ICAL_VEVENT_COMPONENT,
            icalproperty_new_summary(""),
            icalproperty_new_uid(0),
            icalproperty_vanew_dtstart(dtstart,
                icalparameter_new_tzid(tzid),
                0),
            icalproperty_new_duration(iduration),
            0);

    /* Create LOCATION property and add it to the event
    icalcomponent_add_property(event,
        icalproperty_new_location(locate));

    /* Add our event to the calendar
    icalcomponent_add_component(cal, event);

    /* Create a string from our calendar and send it to stdout
    printf("%s", icalcomponent_as_ical_string(cal));
    */

    // Test function to write out
    FILE* file = fopen("event_details.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing\n");
        return;
    }
    fprintf(file, "Event Name: %s\n", name);
    fprintf(file, "Location: %s\n", locate);
    fprintf(file, "Start Hour: %d\n", *shour);
    fprintf(file, "Start Minute: %02d\n", *sminute);
    fprintf(file, "Duration (minutes): %d\n", *duration);
    fprintf(file, "Date: %d-%d-%d \n",
        date->tm_year + 1900, date->tm_mon + 1, date->tm_mday);

    // Close the file
    fclose(file);
}