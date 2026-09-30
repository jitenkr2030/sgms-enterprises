#include "IndustryManager.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
#include <QVariant>

IndustryManager& IndustryManager::instance()
{
    static IndustryManager inst;
    return inst;
}

IndustryManager::IndustryManager()
{
    load();
}

QString IndustryManager::industry() const { return m_industry; }

void IndustryManager::setIndustry(const QString& industry)
{
    m_industry = industry;
    m_labels = labelsFor(industry);
    save();
}

QString IndustryManager::label(const QString& key) const
{
    return m_labels.value(key, key);
}

QMap<QString, QString> IndustryManager::allLabels() const
{
    return m_labels;
}

bool IndustryManager::isFirstLaunch() const
{
    QSqlQuery q;
    q.exec("SELECT value FROM app_settings WHERE key = 'industry'");
    return !q.next();
}

void IndustryManager::save()
{
    QSqlQuery q;
    q.exec("DELETE FROM app_settings WHERE key = 'industry'");
    q.prepare("INSERT INTO app_settings (key, value) VALUES ('industry', ?)");
    q.addBindValue(m_industry);
    q.exec();
}

void IndustryManager::load()
{
    QSqlQuery q;
    q.exec("SELECT value FROM app_settings WHERE key = 'industry'");
    if (q.next()) {
        m_industry = q.value(0).toString();
    } else {
        m_industry = "Security Guards";
    }
    m_labels = labelsFor(m_industry);
}

QStringList IndustryManager::availableIndustries()
{
    return {
        "Security Guards",
        "Housekeeping",
        "Maid Services",
        "Driver Agencies",
        "Facility Management",
        "Cook/Chef Agencies",
        "Bouncer/Bodyguard",
        "Pest Control",
        "Gardener/Landscaping",
        "Nanny/Caretaker",
        "Patient Attendant",
        "Warehouse Staffing",
        "Construction Labour",
        "Hotel Staff Supply",
        "Event Staffing",
        "IT Staff Augmentation",
        "Other"
    };
}

QMap<QString, QString> IndustryManager::labelsFor(const QString& industry)
{
    QMap<QString, QString> labels;

    // DEFAULTS (Security Guards)
    labels["worker_label"] = "Guard";
    labels["worker_label_plural"] = "Guards";
    labels["worker_code_label"] = "Guard Code";
    labels["site_label"] = "Site";
    labels["site_label_plural"] = "Sites";
    labels["site_type_label"] = "Site Type";
    labels["duty_label"] = "Duty";
    labels["duty_label_plural"] = "Duties";
    labels["attendance_label"] = "Attendance";
    labels["salary_label"] = "Salary";
    labels["uniform_label"] = "Uniform";
    labels["equipment_label"] = "Equipment";
    labels["client_label"] = "Client";
    labels["client_label_plural"] = "Clients";
    labels["compliance_label"] = "Compliance";
    labels["training_label"] = "Training";
    labels["app_title"] = "Security Guard Management System";
    labels["app_short_title"] = "SGMS - Security";
    labels["company_type"] = "Security Agency";

    if (industry == "Housekeeping") {
        labels["worker_label"] = "Housekeeper";
        labels["worker_label_plural"] = "Housekeepers";
        labels["worker_code_label"] = "Staff Code";
        labels["site_label"] = "Location";
        labels["site_label_plural"] = "Locations";
        labels["site_type_label"] = "Location Type";
        labels["duty_label"] = "Task";
        labels["duty_label_plural"] = "Tasks";
        labels["uniform_label"] = "Workwear";
        labels["equipment_label"] = "Cleaning Supplies";
        labels["compliance_label"] = "Labour Compliance";
        labels["training_label"] = "Skill Training";
        labels["app_title"] = "Housekeeping Staff Management System";
        labels["app_short_title"] = "SGMS - Housekeeping";
        labels["company_type"] = "Housekeeping Agency";
    }
    else if (industry == "Maid Services") {
        labels["worker_label"] = "Maid";
        labels["worker_label_plural"] = "Maids";
        labels["worker_code_label"] = "Staff ID";
        labels["site_label"] = "Home";
        labels["site_label_plural"] = "Homes";
        labels["site_type_label"] = "Service Type";
        labels["duty_label"] = "Task";
        labels["duty_label_plural"] = "Tasks";
        labels["uniform_label"] = "Workwear";
        labels["equipment_label"] = "Supplies";
        labels["client_label"] = "Employer";
        labels["client_label_plural"] = "Employers";
        labels["compliance_label"] = "Police Verification";
        labels["training_label"] = "Skill Training";
        labels["app_title"] = "Maid & Domestic Staff Management";
        labels["app_short_title"] = "SGMS - Maid Services";
        labels["company_type"] = "Domestic Staff Agency";
    }
    else if (industry == "Driver Agencies") {
        labels["worker_label"] = "Driver";
        labels["worker_label_plural"] = "Drivers";
        labels["worker_code_label"] = "Driver Code";
        labels["site_label"] = "Route";
        labels["site_label_plural"] = "Routes";
        labels["site_type_label"] = "Vehicle Type";
        labels["duty_label"] = "Trip";
        labels["duty_label_plural"] = "Trips";
        labels["uniform_label"] = "Uniform";
        labels["equipment_label"] = "Vehicle Accessories";
        labels["compliance_label"] = "License Compliance";
        labels["training_label"] = "Driver Training";
        labels["app_title"] = "Driver Management System";
        labels["app_short_title"] = "SGMS - Drivers";
        labels["company_type"] = "Transport Agency";
    }
    else if (industry == "Facility Management") {
        labels["worker_label"] = "Staff";
        labels["worker_label_plural"] = "Staff";
        labels["worker_code_label"] = "Staff Code";
        labels["site_label"] = "Facility";
        labels["site_label_plural"] = "Facilities";
        labels["site_type_label"] = "Facility Type";
        labels["duty_label"] = "Service";
        labels["duty_label_plural"] = "Services";
        labels["uniform_label"] = "Uniform";
        labels["equipment_label"] = "Equipment";
        labels["client_label"] = "Client";
        labels["client_label_plural"] = "Clients";
        labels["compliance_label"] = "Compliance";
        labels["training_label"] = "Training";
        labels["app_title"] = "Facility Management System";
        labels["app_short_title"] = "SGMS - Facility";
        labels["company_type"] = "Facility Management Company";
    }
    else if (industry == "Cook/Chef Agencies") {
        labels["worker_label"] = "Cook";
        labels["worker_label_plural"] = "Cooks";
        labels["worker_code_label"] = "Staff ID";
        labels["site_label"] = "Kitchen";
        labels["site_label_plural"] = "Kitchens";
        labels["site_type_label"] = "Service Type";
        labels["duty_label"] = "Meal Service";
        labels["duty_label_plural"] = "Meal Services";
        labels["uniform_label"] = "Kitchen Wear";
        labels["equipment_label"] = "Kitchen Equipment";
        labels["client_label"] = "Client";
        labels["client_label_plural"] = "Clients";
        labels["compliance_label"] = "FSSAI Compliance";
        labels["training_label"] = "Food Safety Training";
        labels["app_title"] = "Cook & Chef Management System";
        labels["app_short_title"] = "SGMS - Cooks";
        labels["company_type"] = "Catering/Cook Agency";
    }
    else if (industry == "Bouncer/Bodyguard") {
        labels["worker_label"] = "Bouncer";
        labels["worker_label_plural"] = "Bouncers";
        labels["worker_code_label"] = "Agent Code";
        labels["site_label"] = "Venue";
        labels["site_label_plural"] = "Venues";
        labels["site_type_label"] = "Venue Type";
        labels["duty_label"] = "Assignment";
        labels["duty_label_plural"] = "Assignments";
        labels["uniform_label"] = "Attire";
        labels["equipment_label"] = "Security Gear";
        labels["compliance_label"] = "PSARA Compliance";
        labels["training_label"] = "Security Training";
        labels["app_title"] = "Bouncer & Bodyguard Management";
        labels["app_short_title"] = "SGMS - Bouncers";
        labels["company_type"] = "Security Service Agency";
    }
    else if (industry == "Pest Control") {
        labels["worker_label"] = "Technician";
        labels["worker_label_plural"] = "Technicians";
        labels["worker_code_label"] = "Technician ID";
        labels["site_label"] = "Service Location";
        labels["site_label_plural"] = "Service Locations";
        labels["site_type_label"] = "Property Type";
        labels["duty_label"] = "Service Job";
        labels["duty_label_plural"] = "Service Jobs";
        labels["uniform_label"] = "Protective Gear";
        labels["equipment_label"] = "Chemicals & Equipment";
        labels["compliance_label"] = "Pesticide License";
        labels["training_label"] = "Safety Training";
        labels["app_title"] = "Pest Control Management System";
        labels["app_short_title"] = "SGMS - Pest Control";
        labels["company_type"] = "Pest Control Agency";
    }
    else if (industry == "Nanny/Caretaker") {
        labels["worker_label"] = "Caretaker";
        labels["worker_label_plural"] = "Caretakers";
        labels["worker_code_label"] = "Staff ID";
        labels["site_label"] = "Home";
        labels["site_label_plural"] = "Homes";
        labels["site_type_label"] = "Care Type";
        labels["duty_label"] = "Care Task";
        labels["duty_label_plural"] = "Care Tasks";
        labels["uniform_label"] = "Workwear";
        labels["equipment_label"] = "Care Supplies";
        labels["client_label"] = "Family";
        labels["client_label_plural"] = "Families";
        labels["compliance_label"] = "Police Verification";
        labels["training_label"] = "Care Training";
        labels["app_title"] = "Nanny & Caretaker Management";
        labels["app_short_title"] = "SGMS - Caretakers";
        labels["company_type"] = "Care Staff Agency";
    }
    else if (industry == "Construction Labour") {
        labels["worker_label"] = "Labourer";
        labels["worker_label_plural"] = "Labourers";
        labels["worker_code_label"] = "Worker Code";
        labels["site_label"] = "Project Site";
        labels["site_label_plural"] = "Project Sites";
        labels["site_type_label"] = "Project Type";
        labels["duty_label"] = "Work Order";
        labels["duty_label_plural"] = "Work Orders";
        labels["uniform_label"] = "Safety Gear";
        labels["equipment_label"] = "Tools & Equipment";
        labels["compliance_label"] = "Labour Compliance";
        labels["training_label"] = "Safety Training";
        labels["app_title"] = "Construction Labour Management";
        labels["app_short_title"] = "SGMS - Labour";
        labels["company_type"] = "Labour Contractor";
    }
    else if (industry == "Event Staffing") {
        labels["worker_label"] = "Event Staff";
        labels["worker_label_plural"] = "Event Staff";
        labels["worker_code_label"] = "Staff Code";
        labels["site_label"] = "Event Venue";
        labels["site_label_plural"] = "Event Venues";
        labels["site_type_label"] = "Event Type";
        labels["duty_label"] = "Event Assignment";
        labels["duty_label_plural"] = "Event Assignments";
        labels["uniform_label"] = "Event Attire";
        labels["equipment_label"] = "Event Equipment";
        labels["compliance_label"] = "Compliance";
        labels["training_label"] = "Event Training";
        labels["app_title"] = "Event Staffing Management System";
        labels["app_short_title"] = "SGMS - Events";
        labels["company_type"] = "Event Staffing Agency";
    }
    else if (industry == "Hotel Staff Supply") {
        labels["worker_label"] = "Hotel Staff";
        labels["worker_label_plural"] = "Hotel Staff";
        labels["worker_code_label"] = "Staff Code";
        labels["site_label"] = "Hotel";
        labels["site_label_plural"] = "Hotels";
        labels["site_type_label"] = "Department";
        labels["duty_label"] = "Shift Assignment";
        labels["duty_label_plural"] = "Shift Assignments";
        labels["uniform_label"] = "Hotel Uniform";
        labels["equipment_label"] = "Hotel Supplies";
        labels["client_label"] = "Hotel";
        labels["client_label_plural"] = "Hotels";
        labels["compliance_label"] = "Compliance";
        labels["training_label"] = "Hospitality Training";
        labels["app_title"] = "Hotel Staff Management System";
        labels["app_short_title"] = "SGMS - Hotels";
        labels["company_type"] = "Hotel Staff Supply Agency";
    }
    else if (industry == "Other") {
        labels["worker_label"] = "Worker";
        labels["worker_label_plural"] = "Workers";
        labels["worker_code_label"] = "Worker Code";
        labels["site_label"] = "Location";
        labels["site_label_plural"] = "Locations";
        labels["site_type_label"] = "Location Type";
        labels["duty_label"] = "Assignment";
        labels["duty_label_plural"] = "Assignments";
        labels["uniform_label"] = "Uniform";
        labels["equipment_label"] = "Equipment";
        labels["client_label"] = "Client";
        labels["client_label_plural"] = "Clients";
        labels["compliance_label"] = "Compliance";
        labels["training_label"] = "Training";
        labels["app_title"] = "Workforce Management System";
        labels["app_short_title"] = "SGMS - Workforce";
        labels["company_type"] = "Staffing Agency";
    }

    return labels;
}
