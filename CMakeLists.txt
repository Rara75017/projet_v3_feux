cmake_minimum_required(VERSION 3.21)
project(feux_tricolores VERSION 3.0.0 LANGUAGES CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
option(FEUX_STATIC_RUNTIME "Link MinGW runtimes statically" ON)
include(CTest)

# Composant 1 : modele et regles de fonctionnement, sans I/O.
add_library(feux_domaine STATIC
    src/domaine/EtatCarrefour.cpp src/domaine/FeuTricolore.cpp
    src/domaine/PlanFeux.cpp src/domaine/ControleurCarrefour.cpp)
target_include_directories(feux_domaine PUBLIC include)
target_compile_features(feux_domaine PUBLIC cxx_std_17)

# Composant 2 : orchestration en temps virtuel et interface d'affichage.
add_library(feux_simulation STATIC src/simulation/Scenario.cpp src/simulation/Simulateur.cpp)
target_link_libraries(feux_simulation PUBLIC feux_domaine)

# Composant 3 : adaptation a la console et a la ligne de commande.
add_library(feux_console STATIC src/console/AffichageConsole.cpp src/console/OptionsLigneCommande.cpp)
target_link_libraries(feux_console PUBLIC feux_simulation)

# Application : point d'assemblage des composants.
add_executable(feux_tricolores src/main.cpp)
target_link_libraries(feux_tricolores PRIVATE feux_console)

function(lab_options target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()
function(lab_runtime target)
    if(MINGW AND FEUX_STATIC_RUNTIME)
        target_link_options(${target} PRIVATE -static)
    endif()
endfunction()
foreach(target IN ITEMS feux_domaine feux_simulation feux_console feux_tricolores)
    lab_options(${target})
endforeach()
lab_runtime(feux_tricolores)

if(BUILD_TESTING)
    add_executable(feux_unit_tests tests/unit/test_main.cpp tests/unit/test_domaine.cpp
                   tests/unit/test_simulation.cpp tests/unit/test_console.cpp)
    target_link_libraries(feux_unit_tests PRIVATE feux_console)
    lab_options(feux_unit_tests)
    lab_runtime(feux_unit_tests)
    set(unit_cases
        console_trace_exacte
        console_urgence
        console_erreur_ecriture
        options_defaut
        options_personnalisees
        options_help
        options_inconnue_refusee
        options_dupliquee_refusee
        options_conversion_stricte
        options_valeur_absente
        feu_initial_rouge
        feu_nom_vide_refuse
        feu_cycle
        feu_orange_depuis_rouge_refuse
        feu_vert_depuis_orange_refuse
        feu_retour_rouge_force
        plan_durees
        plan_zero_refuse
        plan_bornes
        carrefour_initial
        carrefour_frontiere_vert
        carrefour_frontiere_orange
        carrefour_intervalle_tout_rouge
        carrefour_cycle_complet
        carrefour_exclusion_axes
        carrefour_etat_est_une_copie
        urgence_depuis_toutes_phases
        urgence_reprise_par_tout_rouge
        urgence_appels_idempotents
        scenario_bornes
        scenario_reprise_sans_arret_refusee
        scenario_ordre_evenements
        scenario_evenement_hors_duree
        simulation_duree_zero
        simulation_injection_affichage
        simulation_arret_et_reprise
        simulation_arret_initial
        simulation_evenement_final
    )
    foreach(case_name IN LISTS unit_cases)
        add_test(NAME unit.${case_name} COMMAND feux_unit_tests ${case_name})
        set_tests_properties(unit.${case_name} PROPERTIES TIMEOUT 5 LABELS unit)
    endforeach()
endif()
