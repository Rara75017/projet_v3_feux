pipeline {
    agent { label 'windows-lab' }
    options {
        skipDefaultCheckout(true)
        disableConcurrentBuilds()
        skipStagesAfterUnstable()
        timestamps()
        timeout(time: 20, unit: 'MINUTES')
        buildDiscarder(logRotator(numToKeepStr: '20'))
    }
    triggers { pollSCM('H/5 * * * *') }
    parameters {
        booleanParam(name: 'DEPLOY_LOCAL', defaultValue: false,
                     description: 'Deployer dans le laboratoire local')
    }
    stages {
        stage('Sources') {
            steps {
                deleteDir()
                checkout scm
            }
        }
        stage('Environnement') {
            steps {
                bat '''
                    powershell.exe -NoProfile -ExecutionPolicy Bypass ^
                      -File scripts/setup.ps1
                '''
            }
        }
        stage('Diagnostic') {
            steps {
                bat 'powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/doctor.ps1'
            }
        }
        stage('Compilation') {
            steps {
                bat '''
                    powershell.exe -NoProfile -ExecutionPolicy Bypass ^
                      -File scripts/build.ps1
                '''
            }
        }
        stage('Tests') {
            steps {
                bat '''
                    powershell.exe -NoProfile -ExecutionPolicy Bypass ^
                      -File scripts/test.ps1
                '''
            }
            post {
                always {
                    junit testResults: 'reports/unit.xml,reports/junit.xml',
                          allowEmptyResults: false
                }
            }
        }
        stage('Livraison') {
            steps {
                bat '''
                    powershell.exe -NoProfile -ExecutionPolicy Bypass ^
                      -File scripts/package.ps1 -Version b%BUILD_NUMBER%
                '''
                archiveArtifacts artifacts: 'dist/*.zip,dist/*.sha256',
                                 fingerprint: true
            }
        }
        stage('Deploiement local') {
            when { expression { params.DEPLOY_LOCAL } }
            steps {
                bat '''
                    powershell.exe -NoProfile -ExecutionPolicy Bypass ^
                      -File scripts/deploy.ps1 ^
                      -Package "dist/feux-tricolores-b%BUILD_NUMBER%.zip" ^
                      -TargetRoot "%LAB_DEPLOY_ROOT%"
                '''
            }
        }
    }
}
