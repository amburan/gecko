[Mesh]
  type = FileMesh
  file = atc_mesh.e
  dim = 2
  block_id = 1
  block_name = atc_region
[]

[MeshModifiers]
  [./lbc_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = lbc_sideset
    block = 1
    normal = '-1 0 0'
  [../]
  [./rbc_sideset]
    type = SideSetsAroundSubdomain
    new_boundary = rbc_sideset
    block = 1
    normal = '1 0 0'
  [../]
  [./atc_nodeset]
    type = BoundingBoxNodeSet
    top_right = '14 1 1' # {0.1*14/16},{0.01*1},0
    new_boundary = atc_nodeset
    bottom_left = '2 0 0 ' # {0.1*2/16},0,0
  [../]
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
  #active = 'left_DC right_DC'
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
    boundary = atc_nodeset
    lammps_userobject = lammps_uo
  [../]
[]

[Materials]
  active = 'dummy_mat argon'
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
  active = 'lbc rbc'
  [./rbc]
    type = AverageNodalVariableValue
    variable = temp
    boundary = 6
    execute_on = 'timestep_begin timestep_end'
  [../]
  [./lbc]
    type = AverageNodalVariableValue
    variable = temp
    boundary = 5
    execute_on = 'timestep_begin timestep_end'
  [../]
  [./lbc_nodal_val]
    type = NodalVariableValue
    variable = temp
    nodeid = 6
  [../]
[]

[UserObjects]
  #active = ''
  [./lammps_uo]
    type = LammpsUserObject
    lammpsMDInput = '../../../../lammps/examples/COUPLE/simple/in.bar1d_flux_md'
    lammpsEquilibriationInput = '../../../../lammps/examples/COUPLE/simple/in.bar1d_flux_eq'
    #boundary = '5 6' # This is not used, run your application with --error
    leftDownScalingTemperature = lbc
    rightDownScalingTemperature = rbc
    LammpsTimeSteps = 200
    execute_on = timestep_begin
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
