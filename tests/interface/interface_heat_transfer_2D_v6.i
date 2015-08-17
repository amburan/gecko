[Mesh]
  type = FileMesh
  file = atc_mesh_large.e
  dim = 2
  block_id = 1
  block_name = atc_region
[]

[Variables]
  [./temp]
    initial_condition = 10
  [../]
[]

[Kernels]
  [./hc]
    type = HeatConduction
    variable = temp
    block = '0 1'
  [../]
  [./hc_dt]
    type = HeatConductionTimeDerivative
    variable = temp
  [../]
[]

[BCs]
  # active = 'left_DC right_DC'
  [./left_DC]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 50
  [../]
  [./right_DC]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 10
  [../]
  [./multiscale_DirichletBC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = md_region
    lammps_userobject = lammps_uo
  [../]
[]

[Materials]
  active = 'argon'
  [./hcm]
    type = HeatConductionMaterial
    block = 0
    specific_heat = 1
    thermal_conductivity = 1
  [../]
  [./ms_hc_mat]
    type = HeatConductionMaterial
    block = 1
    specific_heat = 1
    thermal_conductivity = 1
  [../]
  [./dummy_mat]
    type = GenericConstantMaterial
    block = 1
  [../]
  [./argon]
    type = GenericConstantMaterial
    block = '0 1'
    prop_names = 'thermal_conductivity specific_heat density'
    prop_values = '0.01772 520.33 .000001784' # W/m*K, J/kg-K, kg/m^3 @ 296K
  [../]
[]

[Postprocessors]
  [./lbc]
    type = SideAverageValue
    variable = temp
    execute_on = 'timestep_begin timestep_end'
    boundary = lbc_sideset
  [../]
  [./rbc]
    type = SideAverageValue
    variable = temp
    execute_on = 'timestep_begin timestep_end'
    boundary = rbc_sideset
  [../]
[]

[UserObjects]
  [./lammps_uo]
    type = LammpsUserObject
    rightDownScalingTemperature = rbc
    leftDownScalingTemperature = lbc
    execute_on = timestep_begin
    LammpsTimeSteps = 1000
    lammpsEquilibriationInput = in.bar1d_flux_eq
  [../]
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Transient
  num_steps = 200
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  [./out]
    type = Exodus
    output_initial = true
  [../]
[]
