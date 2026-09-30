cmake_minimum_required(VERSION 3.20)
project(SecurityGuardManager VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)
find_package(Qt6 QUIET COMPONENTS Charts)
if(Qt6Charts_FOUND)
    message(STATUS "Qt6 Charts found")
else()
    message(STATUS "Qt6 Charts not found - building without charts (OK)")
endif()

set(SOURCES
    src/main.cpp src/database/DatabaseManager.cpp
    src/ui/IndustrySelectorDialog.h
    src/ui/IndustrySelectorDialog.cpp
    src/core/IndustryManager.h
    src/core/IndustryManager.cpp
    src/ui/LoginDialog.cpp src/ui/MainWindow.cpp src/ui/DashboardWidget.cpp
    src/ui/GuardWidget.cpp src/ui/GuardDialog.cpp
    src/ui/ClientWidget.cpp src/ui/ClientDialog.cpp
    src/ui/SiteWidget.cpp src/ui/SiteDialog.cpp
    src/ui/AttendanceWidget.cpp
    src/ui/DutyWidget.cpp src/ui/DutyDialog.cpp
    src/ui/LeaveWidget.cpp src/ui/LeaveDialog.cpp
    src/ui/SalaryWidget.cpp
    src/ui/UniformWidget.cpp src/ui/UniformDialog.cpp
    src/ui/EquipmentWidget.cpp src/ui/EquipmentDialog.cpp
    src/ui/VisitorWidget.cpp src/ui/VisitorDialog.cpp
    src/ui/VehicleWidget.cpp src/ui/VehicleDialog.cpp
    src/ui/IncidentWidget.cpp src/ui/IncidentDialog.cpp
    src/ui/TrainingWidget.cpp src/ui/TrainingDialog.cpp
    src/ui/DocumentWidget.cpp src/ui/DocumentDialog.cpp
    src/ui/ReportsWidget.cpp
    src/ui/ComplaintWidget.cpp
    src/ui/ComplaintDialog.cpp
    src/ui/FineWidget.cpp
    src/ui/FineDialog.cpp
    src/ui/AlertWidget.cpp
    src/ui/PayrollWidget.cpp
    src/ui/AnnouncementWidget.cpp
    src/ui/AnnouncementDialog.cpp
    src/ui/PhotoWidget.cpp
    src/ui/PhotoDialog.cpp
    src/ui/InvoiceWidget.cpp
    src/ui/InvoiceDialog.cpp
    src/ui/TicketWidget.cpp
    src/ui/TicketDialog.cpp
    src/ui/AuditLogWidget.cpp
    src/ui/RoleManagerWidget.cpp
    src/ui/ComplianceWidget.cpp
    src/ui/ComplianceDialog.cpp
    src/ui/LicenseDialog.cpp
    src/ui/PermissionManager.cpp
    src/ui/PhotoWidget.cpp
    src/ui/PhotoDialog.cpp
    src/ui/InvoiceWidget.cpp
    src/ui/InvoiceDialog.cpp
    src/ui/TicketWidget.cpp
    src/ui/TicketDialog.cpp
    src/ui/AuditLogWidget.cpp
    src/ui/RoleManagerWidget.cpp
    src/ui/ComplianceWidget.cpp
    src/ui/ComplianceDialog.cpp
    src/ui/LicenseDialog.cpp
    src/ui/PermissionManager.cpp
    src/ui/AlertWidget.cpp
    src/ui/PayrollWidget.cpp
    src/ui/AnnouncementWidget.cpp
    src/ui/AnnouncementDialog.cpp
    src/ui/PhotoWidget.cpp
    src/ui/PhotoDialog.cpp
    src/ui/InvoiceWidget.cpp
    src/ui/InvoiceDialog.cpp
    src/ui/TicketWidget.cpp
    src/ui/TicketDialog.cpp
    src/ui/AuditLogWidget.cpp
    src/ui/RoleManagerWidget.cpp
    src/ui/ComplianceWidget.cpp
    src/ui/ComplianceDialog.cpp
    src/ui/LicenseDialog.cpp
    src/ui/PermissionManager.cpp
    src/ui/PhotoWidget.cpp
    src/ui/PhotoDialog.cpp
    src/ui/InvoiceWidget.cpp
    src/ui/InvoiceDialog.cpp
    src/ui/TicketWidget.cpp
    src/ui/TicketDialog.cpp
    src/ui/AuditLogWidget.cpp
    src/ui/RoleManagerWidget.cpp
    src/ui/ComplianceWidget.cpp
    src/ui/ComplianceDialog.cpp
    src/ui/LicenseDialog.cpp
    src/ui/PermissionManager.cpp
    src/ui/SearchWidget.cpp
    src/ui/BackupWidget.cpp
    src/ui/SettingsWidget.cpp
    src/ui/StyleManager.cpp
)

set(HEADERS
    src/database/DatabaseManager.h
    src/ui/LoginDialog.h src/ui/MainWindow.h src/ui/DashboardWidget.h
    src/ui/GuardWidget.h src/ui/GuardDialog.h
    src/ui/ClientWidget.h src/ui/ClientDialog.h
    src/ui/SiteWidget.h src/ui/SiteDialog.h
    src/ui/AttendanceWidget.h
    src/ui/DutyWidget.h src/ui/DutyDialog.h
    src/ui/LeaveWidget.h src/ui/LeaveDialog.h
    src/ui/SalaryWidget.h
    src/ui/UniformWidget.h src/ui/UniformDialog.h
    src/ui/EquipmentWidget.h src/ui/EquipmentDialog.h
    src/ui/VisitorWidget.h src/ui/VisitorDialog.h
    src/ui/VehicleWidget.h src/ui/VehicleDialog.h
    src/ui/IncidentWidget.h src/ui/IncidentDialog.h
    src/ui/TrainingWidget.h src/ui/TrainingDialog.h
    src/ui/DocumentWidget.h src/ui/DocumentDialog.h
    src/ui/ReportsWidget.h
    src/ui/ComplaintWidget.h
    src/ui/ComplaintDialog.h
    src/ui/FineWidget.h
    src/ui/FineDialog.h
    src/ui/AlertWidget.h
    src/ui/PayrollWidget.h
    src/ui/AnnouncementWidget.h
    src/ui/AnnouncementDialog.h
    src/ui/PhotoWidget.h
    src/ui/PhotoDialog.h
    src/ui/InvoiceWidget.h
    src/ui/InvoiceDialog.h
    src/ui/TicketWidget.h
    src/ui/TicketDialog.h
    src/ui/AuditLogWidget.h
    src/ui/RoleManagerWidget.h
    src/ui/ComplianceWidget.h
    src/ui/ComplianceDialog.h
    src/ui/LicenseDialog.h
    src/ui/PermissionManager.h
    src/ui/PhotoWidget.h
    src/ui/PhotoDialog.h
    src/ui/InvoiceWidget.h
    src/ui/InvoiceDialog.h
    src/ui/TicketWidget.h
    src/ui/TicketDialog.h
    src/ui/AuditLogWidget.h
    src/ui/RoleManagerWidget.h
    src/ui/ComplianceWidget.h
    src/ui/ComplianceDialog.h
    src/ui/LicenseDialog.h
    src/ui/PermissionManager.h
    src/ui/AlertWidget.h
    src/ui/PayrollWidget.h
    src/ui/AnnouncementWidget.h
    src/ui/AnnouncementDialog.h
    src/ui/PhotoWidget.h
    src/ui/PhotoDialog.h
    src/ui/InvoiceWidget.h
    src/ui/InvoiceDialog.h
    src/ui/TicketWidget.h
    src/ui/TicketDialog.h
    src/ui/AuditLogWidget.h
    src/ui/RoleManagerWidget.h
    src/ui/ComplianceWidget.h
    src/ui/ComplianceDialog.h
    src/ui/LicenseDialog.h
    src/ui/PermissionManager.h
    src/ui/PhotoWidget.h
    src/ui/PhotoDialog.h
    src/ui/InvoiceWidget.h
    src/ui/InvoiceDialog.h
    src/ui/TicketWidget.h
    src/ui/TicketDialog.h
    src/ui/AuditLogWidget.h
    src/ui/RoleManagerWidget.h
    src/ui/ComplianceWidget.h
    src/ui/ComplianceDialog.h
    src/ui/LicenseDialog.h
    src/ui/PermissionManager.h
    src/ui/SearchWidget.h
    src/ui/BackupWidget.h
    src/ui/SettingsWidget.h
    src/ui/StyleManager.h
)

set(RESOURCES src/resources/resources.qrc)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql Qt6::PrintSupport)
if(Qt6Charts_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Charts)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_QT_CHARTS)
endif()
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif()
